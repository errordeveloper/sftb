# include	<stdio.h>
# include	<malloc.h>

# include	<vpi_user.h>
# include	<sndfile.h>

/* This will be the length of the buffer used to hold frames. */
#define		MAX_SIZE	4096

/* Limit to mono files to start with */
#define		MAX_CHAN	1

/* There should be one sftb_s struc, but keep it simple for now! */

struct sftb_s {
const char     *name;	// path name
SNDFILE	       *file;	// file descriptor
SF_INFO		info;	// file metadata
int	data[MAX_SIZE];	// data buffer
sf_count_t	seek;	// next offset
sf_count_t	read;	// current buffer lenght
vpiHandle	func;
};


static struct sftb_s sf;


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

      tf_data.type      = vpiSysFunc;
      tf_data.tfname    = "$sftb_store_sample";
      tf_data.calltf    = sftb_store_sample_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_store_sample";

      vpi_register_systf(&tf_data);

      tf_data.type      = vpiSysFunc;
      tf_data.tfname    = "$sftb_fetch_sample";
      tf_data.calltf    = sftb_fetch_sample_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_fetch_sample";

      vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])() = { sftb_register, 0 };

static PLI_INT32
sftb_open_input_file_calltf(char *name)
{

    sf.name = "./data/mono16@22050.f7620.aif" ;

    sf.func = vpi_handle (vpiSysTfCall, NULL);

    //sf.data = malloc(MAX_SIZE);

    if (! (sf.file = sf_open (sf.name, SFM_READ, &(sf.info))))
    {   /* Open failed so print an error message. */
	vpi_printf ("Not able to open input file %s.\n", sf.name) ;
	/* Print the error message from libsndfile. */
	puts (sf_strerror (NULL)) ;
	return  1 ;
	} ;

    if (sf.info.channels > MAX_CHAN)
    {   vpi_printf ("Not able to process more than %d channel(s)\n", MAX_CHAN) ;
	return  1 ;
	} ;

    vpi_printf("The size of the audio file is %d frames.\n", (int)sf.info.frames);

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
sftb_open_output_file_calltf(char *name)
{
      vpi_printf ("Not implemented (TODO)!\n") ;
      return 0;
}

static PLI_INT32
sftb_close_output_file (char *name)
{
    // sf_close (output_file) ;
    vpi_printf ("Not implemented (TODO)!\n");

    //free (sf.data) ;

    return 0 ;
}

static PLI_INT32
sftb_fetch_sample_calltf (char *name)
{ 
    static sf_count_t i = 0 ;

    vpi_printf("Fetching sample %d.\n", (int)i) ;

    if ( i == 0 ) {


	    if ( sf.read != 0 ) {

		vpi_printf("Seek to %d.\n", (int)sf.seek) ;

	    	sf.seek = sf_seek (sf.file, sf.seek, SEEK_SET) ;

		vpi_printf("Seek returned %d.\n", (int)sf.seek) ;

	    }

	    if ( sf.seek != -1 ) {

    	    	sf.read = sf_read_int (sf.file, sf.data, MAX_SIZE) ;

		if ( sf.read < MAX_SIZE ) {
			sf.seek = 0 ;
		} else {
			sf.seek += sf.read ;
		}


	    } else { vpi_printf("Seek error!\n") ; return -1 ; }
    }

    vpi_printf ("Buffered %d sample frames.\n", (int)sf.read) ;

    s_vpi_value v ;

    v.format = vpiIntVal ;
 
    v.value.integer = sf.data[i] ;

    vpi_put_value (sf.func, &v, NULL, vpiNoDelay) ;

    vpi_printf("Sample value is %d.\n", v.value.integer) ;

    i = (i + sf.info.channels) % sf.read ;

    return 0 ;
}

static PLI_INT32
sftb_store_sample_calltf (char *name)
{
    vpi_printf("Not implemented (TODO)!\n");
    return 0 ;
}
