/*
 * Based on Alessandro Ghedini file at
 * https://gist.github.com/ghedo/963382
 */

#include <alsa/asoundlib.h>
#include <fstream>

using namespace std;

#define PCM_DEVICE "default"

unsigned int getFileSize(char *filePath);

int ringBell() {
	unsigned int pcm, tmp, dir;
	unsigned int rate, channels, seconds;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	char *buff;
	int buff_size, loops;

	rate 	 = atoi("22050");
	channels = atoi("1");
	seconds  = atoi("1");

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0)
		printf("ERROR: Can't open \"%s\" PCM device. %s\n", PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE) < 0)
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0)
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0)
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	snd_pcm_hw_params_get_channels(params, &tmp);
	snd_pcm_hw_params_get_rate(params, &tmp, 0);

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

	char FILE_PATH[] = "./zvonok.wav";
	unsigned int fileSize = getFileSize(FILE_PATH);
	int fd = open(FILE_PATH, O_RDONLY);

	if (fd <= 0) printf("ERROR. Can't open file.\n");
	else {
		for (loops = fileSize / buff_size; loops > 0; loops--) {
			if (pcm = read(fd, buff, buff_size) == 0) {
				printf("Early end of file.\n");
				return 0;
			}

			if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
				printf("XRUN.\n");
				snd_pcm_prepare(pcm_handle);
			} else if (pcm < 0) {
				printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
			}

		}
		close(fd);
	}

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);

	return 0;
}

unsigned int getFileSize(char *filePath) {
	streampos begin, end;
	ifstream myfile(filePath, ios::binary);
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	myfile.close();
	return end - begin;
}
