/*
 * Two drum synths controlled by two capacitive touch sliders
 * The Kick synth is a frequency sweep (disco tom-like). Slide position controls frequency
 * The Hat synth is a noise burst. Slider position controls decay
 * 
 * We're keeping a running average of the cap sensor values to establish a baseline
 * A sharp rise of the values indicates a toggle to on.
 */
void drum_init();
void drum_read();

void drum_init() {
  // Set all touch channels
  // Initialize the Audio Lib drum synth parts
}

void drum_read() {
  // Read all touch channels

  // States:
}

void kick_noteon();
void hat_noteon();
void kick_noteoff();
void hat_noteoff();
