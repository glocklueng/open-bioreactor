byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {
  10,0,0,28};

void setupNetwork() {
  Ethernet.begin(mac, ip);
}










