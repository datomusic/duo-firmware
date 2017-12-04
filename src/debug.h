void print_log() {
  Serial.printf("Audio CPU: %d\n", AudioProcessorUsage());
  Serial.printf("Audio RAM: %d\n", AudioMemoryUsage());
}
