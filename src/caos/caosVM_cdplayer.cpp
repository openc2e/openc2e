#include "caosException.h"
#include "caosVM.h"

/**
 _CD_ EJCT (command)
 %status stub

 Eject the CD tray.
*/
void c_CD_EJCT(caosVM*) {
}

/**
 _CD_ FRQH (integer)
 %status stub

 Returns the average value for the highest frequencies detected in the CD player
 for the current tick.
*/
void v_CD_FRQH(caosVM* vm) {
	vm->result.setInt(0);
}

/**
 _CD_ FRQL (integer)
 %status stub

 Returns the average value for the lowest frequencies detected in the CD player
 for the current tick.
*/
void v_CD_FRQL(caosVM* vm) {
	vm->result.setInt(0);
}

/**
 _CD_ FRQM (integer)
 %status stub

 Returns the average value for the medium frequencies detected in the CD player
 for the current tick.
*/
void v_CD_FRQM(caosVM* vm) {
	vm->result.setInt(0);
}

/**
 _CD_ INIT (command)
 %status stub

 Let the game know that you wish to use the cd player. This will shut down all
 in game sounds and music as the mixer is needed to gauge the frequency spectrum.
*/
void c_CD_INIT(caosVM*) {
}

/**
 _CD_ PAWS (command) on_off (integer)
 %status stub

 Pause the CD player if the parameter is greater than zero, to continue playing
 a previous paused track set the parameter to 1.
*/
void c_CD_PAWS(caosVM* vm) {
	VM_PARAM_INTEGER(on_off);
}

/**
 _CD_ PLAY (command) first_track (integer) last_track (integer)
 %status stub

 Tell the CD Player to play the given track.
*/
void c_CD_PLAY(caosVM* vm) {
	VM_PARAM_INTEGER(last_track);
	VM_PARAM_INTEGER(first_track);
}

/**
 _CD_ SHUT (command)
 %status stub

 Tell the game that you have finished with the cd player. This will reinstate
 the in game sounds and music.
*/
void c_CD_SHUT(caosVM*) {
}

/**
 _CD_ STOP (command)
 %status stub

 Stop the cd player.
*/
void c_CD_STOP(caosVM*) {
}
