package bgu.spl.net.api;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.api.Data;
import bgu.spl.net.api.helperDataStructure.Pair;

public class StompMessagingProtocolImp<T>  implements StompMessagingProtocol<T> {

    int connectionId;
    Connections<T> connections;
    private boolean terminate = false;
    Commands commands;

    public  StompMessagingProtocolImp(){
        this.commands = new ComandsImp();
    }

    @Override
    public void start(int connectionId, Connections<T> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(T message) {
        //message is stompframe like "CONNECT\nlogin:user1\npasscode:password1\n\nHello, STOMP!\0"
        //responses are sent via the connections object send functions (if needed).
        //this method should check the command of the mesaage  and call command object...
        //this method responsible to send massage using connection's
        String response = "";
        //split the frame to command, headers, freamBody
        String frame  = (String) message;
        String[] lines = frame.split("\n");
        String command = lines[0];
        //Header's <Key,Value>
        Map<String, String> headers = new HashMap<>();
        int bodyIndex = 0;
        for (int i = 1; i < lines.length; i++) {
            if (lines[i].isEmpty()) {
              bodyIndex = i + 1;
              break;
            }
            String[] header = lines[i].split(":");
            headers.put(header[0], header[1]);
        }
        //Body String
        StringBuilder body = new StringBuilder();
        for (int i = bodyIndex; i < lines.length; i++) {
            //System.out.println("\n\n LINES[I] " + lines[i] +" \n\n");
            body.append(lines[i]+"\n");
        } 
      //   for (int i = bodyIndex; i < lines.length; i++) {
      //     String[] h = lines[i].split(":");
      //     body.append(h[0]+": " + h[1]);
      // }
        String bodyString = body.toString().replace("\0", "");
        switch (command) {
            case "CONNECT":
             // System.out.println("I'am in connect command");
              response =  commands.connect(headers,connectionId,frame);
              connections.send(connectionId, (T) response);
              break;
            case "SEND":
              response =  commands.send(headers,body.toString(), connectionId);
              if(response.equals("sendMsgToAll")){
                String destination = headers.get("destination");
                ConcurrentLinkedQueue<Pair<String,Integer>> allSub = Data.getAllSubInTopic(destination);
                for (Pair<String,Integer> pair : allSub) {
                  //send Massge to each subscriber in the topic
                  String responseToSub = commands.MESSAGE(destination, bodyString.toString(), pair.getFirst());
                  connections.send(pair.getSecond(), (T) responseToSub);
                }
              }else{
                connections.send(connectionId, (T) response);
              }
              break;
            case "SUBSCRIBE":
              response = commands.subscribe(headers,connectionId);
              connections.send(connectionId, (T) response);
              break;
            case "UNSUBSCRIBE":
              response =  commands.unsubscribe(headers, connectionId, frame);
              connections.send(connectionId, (T) response);
              break;
            case "DISCONNECT":

              response = commands.disconnect(headers, connectionId);
              connections.send(connectionId, (T) response);
              break;
    
            default:
             System.out.println("There Is no command like that ");
          }
        
    }

    @Override
    public boolean shouldTerminate() {
       return this.terminate;
    }



    
}



/*in this way i should receive stomp frame String frome the client After Decode!!!! and response to it
* real example from HW: Command: login 1.1.1.1:2000 meni films
 * String frame that client should send to server = "CONNECT\naccept - version :1.2\host : stomp . cs . bgu . ac . il\nlogin : meni\npasscode : films\0"
 * frame that server send to client :  "CONNECT\naccept - version :1.2\0"
  */