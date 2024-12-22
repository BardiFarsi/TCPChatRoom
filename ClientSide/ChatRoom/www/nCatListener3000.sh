#!/bin/bash

while true; do
  # Use ncat to listen on port 3000
  # When a GET request is received, send the raw content of index.html as the response
  ncat -l 3000 --sh-exec "cat /c/Users/farsi/source/CommunicationProtocols/WebCom/www/index.html"
done


