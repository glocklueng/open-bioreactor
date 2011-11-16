Server server(80);

void httpHeader(Client client, int code, String mimeType) {
  if (code==200) {
    client.println("HTTP/1.1 200 OK");
  } 
  else if (code==404) {
    client.println("HTTP/1.1 404 Not Found");
  }
  client.println("Content-Type: "+mimeType);
  client.println();
}


// What is the maximal length o a URL
int URL_MAX_LENGTH=100;

void setupWebServer() {
    server.begin();
}

void loopWebServer() {
  Client client = server.available();
  if (client) {
    String getString=String(URL_MAX_LENGTH);
    String parameters=String(URL_MAX_LENGTH);
    String url=String(URL_MAX_LENGTH);

    boolean current_line_is_blank = true;
    while (client.connected()) {
      //if (DEBUG) Serial.println("In loopWebServer while-loop \n");
      if (client.available()) {
        char c = client.read();
        if (getString.length() < URL_MAX_LENGTH) {
          getString+=c;
        } 
        if (c == '\n' && current_line_is_blank) {
          // we need to check which page we want to access.
          // currently we only allow something like "/page?parameters";
          if (DEBUG) Serial.println("HTTP get string: "+getString);
          int pos = getString.indexOf("/");
          if (pos>0) {
            url=getString.substring(pos+1);

            pos=url.indexOf(" ");
            if (pos>-1) {
              url=url.substring(0,pos);
            }
            pos=url.indexOf("?");
            if (pos>-1) {
              parameters=url.substring(pos+1);
              url=url.substring(0,pos);
            }
          }


          if (DEBUG) {
            Serial.println("URL: "+url);
            Serial.println("Parameters: "+parameters);
          }

          if (url=="") {
          getHomePage(client);

          } 
          else if (url=="SDinfo") {
            sendSDStatusToClient(client);
          } 
          else if (url=="setTargetTemperature") {
            setTargetTemperature(client, parameters);
          } 
          else if (url=="status.txt") { 
          httpHeader(client, 200,"text/text");   
            getStatus( &client);
          } 
          else if (url=="status") { 
          httpHeader(client, 200,"text/html");   
            client.println("<pre>");
            getStatus( &client);
            client.println("</pre>");
          }   
          else { // we look on the SD card
            
            char filename[URL_MAX_LENGTH];
            url.toCharArray(filename, URL_MAX_LENGTH);
            if (DEBUG) Serial.println("Filename: "+url);

            // TODO: check if directory !!!!

            File dataFile = SD.open(filename);

            // if the file is available, write to it:
            if (dataFile) {
              if (url.endsWith("htm")) {
              httpHeader(client, 200,"text/html");
            } 
            else {
              httpHeader(client, 200,"text/text");
            }
              while (dataFile.available()) {
                client.println(dataFile.read());
              }
              dataFile.close();
            }  
            // if the file isn't open, pop up an error:
            else {
              Serial.println("File not found: "+url);
              httpHeader(client, 404,"text/text");
              client.println("File not found: "+url);

            } 
          } 

          break;     

        }
        if (c == '\n') {
          current_line_is_blank = true;
        } 
        else if (c != '\r') {
          current_line_is_blank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}




