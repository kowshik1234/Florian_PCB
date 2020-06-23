struct Phone
{
  String macid;
  int channel;
  signed signalstrength;
  long lastDiscoveredTime;
};

void start_tracking () {
  Phone visitors[1000];
}

void add_visitor (String message){
  if (debug) Serial.print ("Adding visitor ");Serial.println(message);
  
}
