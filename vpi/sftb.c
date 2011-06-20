# include	<stdio.h>
# include	<malloc.h>

# include	<vpi_user.h>
# include	<sndfile.h>

/* This will be the length of the buffer used to hold frames. */
#define		MAX_SIZE	4096

/* Limit to mono files to start with */
#define		MAX_CHAN	1

/* There should be one sftb_s struc, but keep it simple for now! */

typedef struct {
vpiHandle       name;	// path name
SNDFILE	       *file;	// file descriptor
SF_INFO		info;	// file metadata
int	data[MAX_SIZE];	// data buffer
sf_count_t	seek;	// next offset in frames
sf_count_t	read;	// current buffer lenght
vpiHandle	call;
vpiHandle	scan;
} s_sftb_misc ;


static s_sftb_misc 	sf ;
static s_vpi_value	tb ;

/* Pointer to the buffer, will allocate it for a small file. */

/*
 * Retrieving values from Verilog
 *
 * s_vpi_value v;
 * v.format = vpiIntVal;
 * vpi_get_value (handle, &v);
 *
 * value is in v.value.integer
 *
 * Writing values to Verilog
 * 
 * s_vpi_value v;
 * v.format = vpiIntVal;
 * v.value.integer = 0x1234;
 * vpi_put_value (handle, &v, NULL, vpiNoDelay);
 */

static PLI_INT32 sftb_open_input_file_calltf (char *name) ;
static PLI_INT32 sftb_open_output_file_calltf (char *name) ;

/* Fetch Audio Data from SNDFILE */
static PLI_INT32 sftb_fetch_sample_calltf (char *name) ;
/* Store Audio Data into SNDFILE */
static PLI_INT32 sftb_store_sample_calltf (char *name) ;

void sftb_register()
{
      s_vpi_systf_data tf_data;

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_open_input_file";
      tf_data.calltf    = sftb_open_input_file_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_open_input_file";

      vpi_register_systf(&tf_data);

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_open_output_file";
      tf_data.calltf    = sftb_open_output_file_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_open_output_file";

      vpi_register_systf(&tf_data);

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_store_sample";
      tf_data.calltf    = sftb_store_sample_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_store_sample";

      vpi_register_systf(&tf_data);

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_fetch_sample";
      tf_data.calltf    = sftb_fetch_sample_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_fetch_sample";

      vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])() = { sftb_register, 0 };

static PLI_INT32
sftb_open_input_file_calltf(char *f)
{

    //sf.name = "./data/mono16@22050.f7620.aif" ;

    sf.call = vpi_handle (vpiSysTfCall, NULL);
    sf.scan = vpi_iterate (vpiArgument, sf.call);

    sf.name = vpi_scan(sf.scan) ;

    if ( sf.name == 0 ) {
	vpi_printf ("%s: first parameter is missing.\n", f) ;
	vpi_control(vpiFinish, 1) ;

    } else {
	    switch(vpi_get(vpiType, sf.name)) {

	    case vpiConstant:
	    case vpiStringConst:
	        break;
	    default:
		vpi_printf ("%s: first parameter must be a string.\n", f) ;
		vpi_control(vpiFinish, 1) ;
		return -1 ;
	    break;
	    }
    }

    vpi_free_object (sf.scan) ;

    tb.format = vpiStringVal;
    vpi_get_value (sf.name, &tb) ;

    if ( !( sf.file = sf_open (tb.value.str, SFM_READ, &(sf.info)) ) ) {

	vpi_printf ("%s: not able to open input file %s.\n",
		     f, tb.value.str) ;
	vpi_control(vpiFinish, 1) ;
	return  1 ;
    }

    if (sf.info.channels > MAX_CHAN) {
	vpi_printf ("%s: not able to process %d channel(s).\n",
		    f, (int)sf.info.channels) ;
	vpi_printf ("%s: my limit is %d channel(s).\n", f, MAX_CHAN) ;
	vpi_control(vpiFinish, 1) ;
	return  1 ;
    }

    vpi_printf("%s: %s\n", f, tb.value.str) ;
    vpi_printf("%s: the lenght of this audio file is %d frames.\n",
	       f, (int)sf.info.frames) ;

    sf.read = 0 ;
    sf.seek = 0 ;

    return 0 ;
}

static PLI_INT32
sftb_close_input_file (char *name)
{
    sf_close (sf.file) ;

    //free (sf.data) ;

    return 0 ;
}


static PLI_INT32
sftb_open_output_file_calltf(char *f)
{
      vpi_printf ("%s: Not implemented (TODO)!\n", f) ;
      return 0;
}

static PLI_INT32
sftb_close_output_file (char *f)
{
    // sf_close (output_file) ;
    vpi_printf ("%s: Not implemented (TODO)!\n", f);

    //free (sf.data) ;

    return 0 ;
}

static PLI_INT32
sftb_fetch_sample_calltf (char *f)
{ 
    static sf_count_t i = 0 ;

    vpi_printf("%s: fetching sample %d;\n", f, (int)i) ;

    if ( i == 0 ) {


	    if ( sf.read != 0 ) {

		vpi_printf ("%s:sf_seek(..., %d, ...) =", f, (int)sf.seek) ;

	    	sf.seek = sf_seek (sf.file, sf.seek, SEEK_SET) ;

		vpi_printf (">> %d;\n", (int)sf.seek) ;

	    }

	    if ( sf.seek != -1 ) {

    	    	sf.read = sf_read_int (sf.file, sf.data, MAX_SIZE) ;

		if ( sf.read < MAX_SIZE ) {
			sf.seek = 0 ;
		} else {
			sf.seek += sf.read ;
		}


	    } else {
		    vpi_printf ("%s: seek error!\n", f) ;
		    vpi_control(vpiFinish, 1) ;
		    return -1 ;
	    }
    }

    vpi_printf ("%s: buffered %d samples;\n", f, (int)sf.read) ;

    /* parse arguments (which are names of wires)
     * and, if not 0, and there is a channel for
     * it then the channel value will passed on
     * the given wire, if 0, skip the corresponding
     * channel. If it's a mono sound file and more
     * then one wire argument is give, write the
     * same data to all non-zero wires.
     * If we get four wires and only three channels,
     * the fourth wire will be receive zeros.
    */


    tb.format = vpiIntVal ;
 
    tb.value.integer = sf.data[i] ;

    vpi_put_value (sf.call, &tb, NULL, vpiNoDelay) ;

    vpi_printf("%s: sample value is %d;\n", f, tb.value.integer) ;

    i = (i + sf.info.channels) % sf.read ;

    return 0 ;
}

static PLI_INT32
sftb_store_sample_calltf (char *f)
{
    vpi_printf("Not implemented (TODO)!\n");
    return 0 ;
}
