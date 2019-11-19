/* include this into your dwmstatus.c and use get_vol() as volume.
 * if your audio card and subunit numbers differ from 0,0 you might have
 * to use amixer, aplay and the /proc/asound file tree to adapt.
 *
 * I had compilation issues. As result i had to drop the -std=c99 and
 * -pedantic flags from the config.mk
 */

#include <alsa/asoundlib.h>
#include <alsa/control.h>

#include "../util.h"

#define VOLUME_CONTROL "Master Playback Volume"
#define MUTE_CONTROL   "Master Playback Switch"

const char *alsa_volume_perc(void) {
	int volmax;
	int vol;

	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_info_t *info;

	// card and subdevice: /proc/asound/, aplay -L
	snd_hctl_open(&hctl, "hw:0", 0);
	snd_hctl_load(hctl);

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

	snd_ctl_elem_id_set_name(id, VOLUME_CONTROL);
	snd_hctl_elem_t *elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_value_set_id(control, id);

	snd_hctl_elem_read(elem, control);
	snd_hctl_elem_info(elem, info);

	vol = (int)snd_ctl_elem_value_get_integer(control, 0);
	volmax = (int)snd_ctl_elem_info_get_max(info);
	vol = ((100.0 * (vol)) / volmax);

	snd_hctl_close(hctl);
	return bprintf("%02d", vol);
}

const char *alsa_volume_status(void) {
	const char *status;

	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;

	// card and subdevice: /proc/asound/, aplay -L
	snd_hctl_open(&hctl, "hw:0", 0);
	snd_hctl_load(hctl);

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

	snd_ctl_elem_id_set_name(id, MUTE_CONTROL);
	snd_hctl_elem_t *elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);
	snd_hctl_elem_read(elem, control);

	status = snd_ctl_elem_value_get_boolean(control, 0) ? "" : "M";
	snd_hctl_close(hctl);

	return status;
}

