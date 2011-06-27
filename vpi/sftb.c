# include	"util.h"

//# include	<math.h>
//# include	<stdio.h>
//# include	<malloc.h>

# include	<vpi_user.h>
# include	<sndfile.h>

/* This will be the length of the buffer used to hold frames. */
# define		MAX_SIZE	4096

/* Limit to mono files to start with */
# define		MAX_CHAN	32

/* Limit to 32bit integers for now */
# define		MAX_BITS	8*sizeof(int)

/* Default file name */
# define		DEF_FILE	"../data/mono16@22050.f7620.aif"


/* There should be one sftb_s struc, but keep it simple for now! */

/* alternative:
 * {
 *
 * file {
 * 	 handle // function argument
 * 	 string // filename string
 * 	 system // file descriptor
 * 	 offset // next seek offset
 * 	 length // buffer length
 * 	}
 *
 * func {
 * 	 flag
 * 	 call
 * 	 scan
 * 	}
 *
 * wire {
 * 	 link // ?
 * 	 mask // bit mask of 32 bits
 * 	 data // the sample buffer
 * 	 type
 * 	 size
 * 	}
 * } */

typedef struct {
vpiHandle	call;	// function object
vpiHandle	scan;	// argument iterator
PLI_INT16	flag;	// status flag

SNDFILE *	file;	// file descriptor
sf_count_t	seek;	// next offset in frames
sf_count_t	read;	// current buffer lenght
PLI_INT32	mask;	// next seek offset

vpiHandle	wire[MAX_CHAN];
signed int	data[MAX_SIZE];	// sample buffer

union {
vpiHandle       handle;	// file path argument
const char *	string;
              } name ;
SF_INFO		info;	// file metadata
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

static PLI_INT16 sftb_count_arguments (char *func);

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

  sf.flag = 1 ;
  sf.seek = 0 ;

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
    ERROR(3); }

  PRINT ("opened '%s'.\n", sf.name.string) ;
  PRINT ("this file has %d frames.\n", (int)sf.info.frames) ;

  sf.read = sf.seek = 0 ; ERROR(0) ;

EXIT:

   if (sf.scan != NULL) vpi_free_object(sf.scan) ;

   vpi_free_object (sf.call) ;

   if (sf.flag) vpi_control (vpiFinish, 1) ;

   return sf.flag ;
}

static PLI_INT32
sftb_close_input_file_calltf (char *func)
{
    PRINT ("Closing input file '%s'.\n", sf.name.string) ;
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
    static PLI_INT16 x = 0, h = 0 ;

    PLI_INT16 f = sf.info.channels ;

/* Description of local variebles:
 * 
 * x	- output wire count
 * i	- sample index for current buffer
 * f	- sample index for current frame
 * h	- frame helper variable
 *
 */

    DEBUG ("fetching sample %d;\n", (int)i) ;

    if ( i == 0 ) {

	    if ( sf.read != 0 ) {

		DEBUG (" :sf_seek(..., %d, ...) =", (int)sf.seek) ;

	    	sf.seek = sf_seek (sf.file, sf.seek, SEEK_SET) ;

		#if DBG
		vpi_printf (">> %d;\n", (int)sf.seek) ;
		#endif

	    }

	    if ( sf.seek != -1 ) {

    	    	sf.read = sf_read_int (sf.file, sf.data, MAX_SIZE) ;

		DEBUG ("buffered %d samples;\n", (int)sf.read) ;

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

    if ( x == 0 ) {

      if ( 0 ==( x = sftb_count_arguments(func) ) ) {
        vpi_control(vpiFinish, 1) ;
        return -1; }

    /* This also needs to be done only once */
    tb.format = vpiIntVal ;

    }

    /* Check x againg sf.info.channels */

    if ( x == sf.info.channels ) {
      h = sf.info.channels ;
    } else if ( x > sf.info.channels ) {
      h = sf.info.channels ;
    } else if ( x < sf.info.channels ) {
      h = x ;
    }
 
    /* Need to decide what loop construct to use here */

    /*
     * => data[i+f] * getb(mask, h) ???
     */

    //DEBUG ("sf[%d] = %d\n", i, sf.data[i]) ;

    tb.value.integer = (getb(sf.mask, 0))*(sf.data[i]) ;

    //DEBUG ("tb[%d] = %d\n", i, tb.value.integer) ;

    vpi_put_value (sf.wire[0], &tb, NULL, vpiNoDelay) ;

    i = (i + sf.info.channels) % sf.read ;

    return 0 ;
}

static PLI_INT32
sftb_store_sample_calltf (char *func)
{
    PRINT ("Not implemented (TODO)!\n") ;
    return 0 ;
}

static PLI_INT16
sftb_count_arguments (char *func)
{
  static int x, d, s, t ;

  x = sf.mask = 0 ;

/* Description of local variebles:
 *
 * x	- wire index
 * d	- debug index
 * t	- wire type
 * s	- wire size
 *
 */

  sf.call = vpi_handle (vpiSysTfCall, 0) ;
	
  if ( NULL ==( sf.scan = vpi_iterate (vpiArgument, sf.call) ) )
	{
	  PRINT ("zero parameters supplied.\n") ;
	  vpi_free_object (sf.call) ;
	  vpi_control (vpiFinish, 1) ; 
	  return 0; }

  do { DEBUG ("expecting argument %d.\n", x) ;

	if ( NULL !=( sf.wire[x] = vpi_scan (sf.scan) ) ) {

	
	  t = vpi_get(vpiType, sf.wire[x]) ;
	  s = vpi_get(vpiSize, sf.wire[x]) ;

          DEBUG ("vpiType of sf.wire[%d] is %d.\n", x, t ) ;
          DEBUG ("vpiSize of sf.wire[%d] is %d.\n", x, s ) ;

	  /* TODO: implement checking that Net/Reg names are uniq */

	  switch(t) {
            case vpiNet:
            if (s==(MAX_BITS))
		  Sbit(sf.mask, x) ;
	    else { PRINT ("unsupported size of vpiNet!\n") ;
		   /*Cbit(sf.mask, x);*/ }
		  break;
	    case vpiReg:
	    if (s==(MAX_BITS))
		  Sbit(sf.mask, x) ;
	    else { PRINT ("unsupported size of vpiNet!\n") ;
		   /*Cbit(sf.mask, x);*/ }
		  break;
	    case vpiNetArray:
		  PRINT ("vpiNetArray type is not implemented!\n") ;
		  /*Cbit(sf.mask, x);*/
		  break;
            case vpiRegArray:
		  PRINT ("vpiRegArray type is not implemented!\n") ;
		  /*Cbit(sf.mask, x);*/
		  break;
	    default:
		  /*Cbit(sf.mask, x);*/
		  PRINT ("vpiType %d is not supported!\n", t) ;
		  break;
          }

	/* TODO: invistegate the types :) */

	
	} else { DEBUG ("argument %d is not supplied!\n", x) ;
	         break ;
	}

  } while ( (x = ((++x)%MAX_CHAN)) ) ;
  /* Srick to MAX_CHAN here, but use sf.info.channels
   * in the other (fetch/store) functions */

#if DBG
  DEBUG ( "the number of wires is %d.\n", x) ;
  DEBUG ( "sf.mask = 32'b" );

  for ( d = 0 ; d < MAX_CHAN ; d++ ) {
	vpi_printf ("%d", getb(sf.mask, d)) ;
      } vpi_printf ("\n") ;
#endif

  return x;

}
