void getHomePage(Client client) {
    httpHeader(client, 200,"text/html");
    client.println("<a href='status.txt'>Status</a><br>");
    client.println("<a href='SDinfo'>SDinfo</a><br>");
  client.println("<a href='setTargetTemperature?111'>setTargetTemperature?111</a><br>");
}
