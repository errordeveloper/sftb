# include	<stdio.h>

# include	<vpi_user.h>
# include	<sndfile.h>

/* This will be the length of the buffer used to hold frames. */
#define		BUFFER_LEN	1024

/* Limit to mono files to start with */
#define		MAX_CHANNELS	1

/* There should be one sftb_s struc, but keep it simple for now! */

SNDFILE		*input_file;
SF_INFO		sfinfo ;

int		frames ;


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

static void sftb_open_input_file_calltf (char *name) ;
static void sftb_open_output_file_calltf (char *name) ;

/* Fetch Audio Data from SNDFILE */
static void sftb_put_sample_calltf (int *data) ;
/* Store Audio Data into SNDFILE */
static void sftb_get_sample_calltf (int *data) ;

void sftb_register()
{
      s_vpi_systf_data tf_data;

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_open_input_file";
      tf_data.calltf    = sftb_open_intput_file_calltf;
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
      tf_data.tfname    = "$sftb_get_sample";
      tf_data.calltf    = sftb_open_intput_file_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_get_sample";

      vpi_register_systf(&tf_data);

      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$sftb_put_sample";
      tf_data.calltf    = sftb_open_output_file_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = "$sftb_put_sample";

      vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])() = { sftb_register, 0 };

int sftb_open_input_file(char *)
{
    static int data [BUFFER_LEN] ;


    const char	*input_file_name = "./data/mono16@22050.f7620.aif" ;

    /* Here's where we open the input file. We pass sf_open the file name and
    ** a pointer to an SF_INFO struct.
    ** On successful open, sf_open returns a SNDFILE* pointer which is used
    ** for all subsequent operations on that file.
    ** If an error occurs during sf_open, the function returns a NULL pointer.
	**
	** If you are trying to open a raw headerless file you will need to set the
	** format and channels fields of sfinfo before calling sf_open(). For
	** instance to open a raw 16 bit stereo PCM file you would need the following
	** two lines:
	**
	**		sfinfo.format   = SF_FORMAT_RAW | SF_FORMAT_PCM_16 ;
	**		sfinfo.channels = 2 ;
    */
    if (! (input_file = sf_open (input_file_name, SFM_READ, &sfinfo)))
    {   /* Open failed so print an error message. */
	printf ("Not able to open input file %s.\n", input_file_name) ;
	/* Print the error message from libsndfile. */
	puts (sf_strerror (NULL)) ;
	return  1 ;
	} ;

    if (sfinfo.channels > MAX_CHANNELS)
    {   printf ("Not able to process more than %d channel(s)\n", MAX_CHANNELS) ;
	return  1 ;
	} ;

    frames = sf_read_int (input_file, data, BUFFER_LEN);

    printf ("Read %d sample frames.\n", frames) ;

    return 0 ;
}

void sftb_close_input_file ()
{
    sf_close (input_file) ;

    return 0 ;
}

void sftb_close_output_file ()
{
    // sf_close (output_file) ;

    return 0 ;
}


static int sftb_open_input_file_calltf(char *user_data)
{
      vpi_printf("Not implemented (TODO)!\n");
      return 0;
}


static void
sftb_put_data (int *data)
{ 
    static unsigned int i = 0 ;

    s_vpi_value v ;

    v.format = vpiIntVal ;
 
    if (i == frames) {

	i = 0 ;

	/* Perhaps we need to pad it with zeros ... */

    } // else { }

	v.value.integer = data[i]

    i += channels;

    return ;
} /* process_data */
