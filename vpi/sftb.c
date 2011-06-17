# include	<stdio.h>

# include	<vpi_user.h>
# include	<sndfile.h>

/* This will be the length of the buffer used to hold frames. */
#define		BUFFER_LEN	1024

/* Limit to mono files to start with */
#define		MAX_CHANNELS	1

/* There should be one sftb_s struc, but keep it simple for now! */

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
{   /* This is a buffer of double precision floating point values
    ** which will hold our data while we process it.
    */
    static double data [BUFFER_LEN] ;

    /* A SNDFILE is very much like a FILE in the Standard C library. The
    ** sf_open function return an SNDFILE* pointer when they sucessfully
	** open the specified file.
    */
    SNDFILE      *infile, *outfile ;

    /* A pointer to an SF_INFO stutct is passed to sf_open.
    ** On read, the library fills this struct with information about the file.
    ** On write, the struct must be filled in before calling sf_open.
    */
    SF_INFO		sfinfo ;
    int			readcount ;
    const char	*infilename = "./data/mono16@22050.f7620.aif" ;
    //const char	*outfilename = "output.wav" ;

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
    if (! (infile = sf_open (infilename, SFM_READ, &sfinfo)))
    {   /* Open failed so print an error message. */
	printf ("Not able to open input file %s.\n", infilename) ;
	/* Print the error message from libsndfile. */
	puts (sf_strerror (NULL)) ;
	return  1 ;
	} ;

    if (sfinfo.channels > MAX_CHANNELS)
    {   printf ("Not able to process more than %d channels\n", MAX_CHANNELS) ;
	return  1 ;
	} ;
    /* Open the output file. */
    /*
    if (! (outfile = sf_open (outfilename, SFM_WRITE, &sfinfo)))
    {   printf ("Not able to open output file %s.\n", outfilename) ;
	puts (sf_strerror (NULL)) ;
	return  1 ;
	} ;
    */

    /* While there are.frames in the input file, read them, process
    ** them and write them to the output file.
    */
    while ((readcount = sf_read_double (infile, data, BUFFER_LEN)))
    {   process_data (data, readcount, sfinfo.channels) ;
	sf_write_double (outfile, data, readcount) ;
	} ;

    /* Close input and output files. */
    sf_close (infile) ;
    sf_close (outfile) ;

    return 0 ;
} /* main */

static int sftb_open_input_file_calltf(char *user_data)
{
      vpi_printf("Not implemented (TODO)!\n");
      return 0;
}


static void
sftb_put_data (double *data, int count, int channels)
{

//double channel_gain [MAX_CHANNELS] = { 0.5, 0.8, 0.1, 0.4, 0.4, 0.9 } ;

    int k, chan ;

    /* Process the data here.
    ** If the soundfile contains more then 1 channel you need to take care of
    ** the data interleaving youself.
    ** Current we just apply a channel dependant gain.
    */

    for (chan = 0 ; chan < channels ; chan ++)
	for (k = chan ; k < count ; k+= channels)
	    //data [k] *= channel_gain [chan] ;

    return ;
} /* process_data */


