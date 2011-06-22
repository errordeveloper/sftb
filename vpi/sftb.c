# include	<stdio.h>
//# include	<malloc.h>

# include	<vpi_user.h>
# include	<sndfile.h>

/* This will be the length of the buffer used to hold frames. */
# define		MAX_SIZE	4096

/* Limit to mono files to start with */
# define		MAX_CHAN	4

/* Default file name */
# define		DEF_FILE	"/dev/zero" // "./data/mono16@22050.f7620.aif"

# define		PRINT(...) vpi_printf("%s: ",func) ; \
				   vpi_printf(__VA_ARGS__)

# define		ERROR(n)   sf.exit = n ; goto EXIT ;

/* There should be one sftb_s struc, but keep it simple for now! */

typedef struct {
SNDFILE *	file;	// file descriptor
SF_INFO		info;	// file metadata
short		exit;
sf_count_t	seek;	// next offset in frames
sf_count_t	read;	// current buffer lenght
vpiHandle	call;
vpiHandle	scan;
vpiHandle	wire[MAX_CHAN];
int		data[MAX_SIZE];
union {
vpiHandle       handle;	// file path argument
const char *	string;
} name ;
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

static PLI_INT32 sftb_open_input_file_calltf (char *func) ;
static PLI_INT32 sftb_open_output_file_calltf (char *func) ;
static PLI_INT32 sftb_close_input_file_calltf (char *func) ;
static PLI_INT32 sftb_close_output_file_calltf (char *func) ;

/* Fetch Audio Data from SNDFILE */
static PLI_INT32 sftb_fetch_sample_calltf (char *func) ;
/* Store Audio Data into SNDFILE */
static PLI_INT32 sftb_store_sample_calltf (char *func) ;

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
      tf_data.tfname    = "$sftb_close_input_file";
      tf_data.calltf    = sftb_close_input_file_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_close_input_file";

      vpi_register_systf(&tf_data);

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_close_output_file";
      tf_data.calltf    = sftb_close_output_file_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_close_output_file";

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
sftb_open_input_file_calltf(char *func)
{

/* Handle the arguments, if none - use DEF_FILE */

  sf.exit = sf.seek = 0 ;

  sf.call = vpi_handle (vpiSysTfCall, NULL) ;

  if ( NULL !=( sf.scan = vpi_iterate (vpiArgument, sf.call) ) ) {
	
    sf.name.handle = vpi_scan (sf.scan) ;

    switch(vpi_get(vpiType, sf.name.handle))
    {
      case vpiConstant:
      case vpiStringConst:
        break;

      default:
        PRINT ("first parameter is not a string.\n") ;
	ERROR(1);
    }


    tb.format = vpiStringVal ;
    vpi_get_value (sf.name.handle, &tb) ;
    vpi_free_object (sf.name.handle) ;
    sf.name.string = tb.value.str ;

  } else {
  
    sf.name.string = DEF_FILE ;
  
    PRINT ("zero parameters supplied.\n") ;
    PRINT ("using default file name '%s'.\n", sf.name.string) ;

  }

  /* Open the audio file, more error checking may be needed */

  if ( !( sf.file = sf_open (sf.name.string, SFM_READ, &(sf.info)) ) )
  {
    PRINT ("cannot open input file '%s'.\n", sf.name.string) ;
    ERROR(2); }

  if (sf.info.channels > MAX_CHAN)
  {
    PRINT ("cannot process %d channel(s).\n", (int)sf.info.channels) ;
    PRINT ("my limit is %d channel(s).\n", MAX_CHAN) ;
    ERROR(2); }

  PRINT ("opened '%s'.\n", sf.name.string) ;
  PRINT ("this file has %d frames.\n", (int)sf.info.frames) ;

  sf.read = sf.seek = 0 ; ERROR(0);

EXIT:

    switch (sf.exit)
    {
	case 0:
	vpi_free_object (sf.call) ;
	vpi_free_object (sf.scan) ;
	case 1:
	vpi_free_object (sf.call) ;
	vpi_free_object (sf.scan) ;
	case 2:
	vpi_free_object (sf.call) ;
    }

   if (sf.exit) vpi_control (vpiFinish, 1);

   return sf.exit ;
}

static PLI_INT32
sftb_close_input_file_calltf (char *func)
{
    sf_close (sf.file) ;
    // free (sf.data) ;
    return 0 ;
}


static PLI_INT32
sftb_open_output_file_calltf(char *func)
{
  PRINT ("Not implemented (TODO)!\n") ;
  return 0;
}

static PLI_INT32
sftb_close_output_file_calltf (char *func)
{
  // sf_close (output_file) ;
  PRINT ("Not implemented (TODO)!\n") ;
  // free (sf.data) ;
  return 0 ;
}

static PLI_INT32
sftb_fetch_sample_calltf (char *func)
{ 
    static sf_count_t i = 0 ;

    PRINT ("fetching sample %d;\n", (int)i) ;

    if ( i == 0 ) {

	    if ( sf.read != 0 ) {

		PRINT (" :sf_seek(..., %d, ...) =", (int)sf.seek) ;

	    	sf.seek = sf_seek (sf.file, sf.seek, SEEK_SET) ;

		vpi_printf (">> %d;\n", (int)sf.seek) ;

	    }

	    if ( sf.seek != -1 ) {

    	    	sf.read = sf_read_int (sf.file, sf.data, MAX_SIZE) ;

		PRINT ("buffered %d samples;\n", (int)sf.read) ;

		if ( sf.read < MAX_SIZE ) {
			sf.seek = 0 ;
		} else {
			sf.seek += ( sf.read / sf.info.channels ) ;
		}


	    } else {
		    PRINT ("seek error!\n") ;
		    vpi_control(vpiFinish, 1) ;
		    return -1 ;
	    }
    }


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


    sf.call = vpi_handle (vpiSysTfCall, 0) ;
    
    if ( NULL ==( sf.scan = vpi_iterate (vpiArgument, sf.call) ) )
    {
      PRINT ("zero parameters supplied.\n") ;
      vpi_free_object (sf.call) ;
      vpi_control (vpiFinish, 1) ; 
      return 1; }

    for (int k = 0; k < MAX_CHAN; k++) {

    PRINT("expecting wire %d.\n", k) ;

    if ( NULL != ( sf.wire[k] = vpi_scan (sf.scan) ) ) {

    PRINT("it's %d\n", vpi_get(vpiType, sf.wire[k])) ; }

    /* TODO: invistegate the types :) */
    
    }


    tb.format = vpiIntVal ;
 
    tb.value.integer = sf.data[i] ;

    vpi_put_value (sf.call, &tb, NULL, vpiNoDelay) ;

    //PRINT ("sample value is %d;\n", tb.value.integer) ;

    i = (i + sf.info.channels) % sf.read ;

    return 0 ;
}

static PLI_INT32
sftb_store_sample_calltf (char *func)
{
    PRINT ("Not implemented (TODO)!\n") ;
    return 0 ;
}


