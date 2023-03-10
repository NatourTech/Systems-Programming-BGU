package bgu.spl.net.api;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

//we added
import bgu.spl.net.api.Commands;
import bgu.spl.net.api.helperDataStructure.Pair;
import java.util.Random;
public class ComandsImp implements Commands {
    Data data = Data.getInstance();
    private String UserStatus;
    private static final Random RANDOM = new Random();
    //command's come from client!
    @Override
    public String connect(Map<String, String> headersConnect, int connectionId, String fullFrame) {
        String acceptVer  = headersConnect.get("accept-version");
        String host  = headersConnect.get("host");
        String login = headersConnect.get("login");
        String passcode = headersConnect.get("passcode");
        String response = "";

        //we connect set the UserStatus as active
        UserStatus  = login;
        //New User
        if(!data.isNewUser(login)){
            data.getClietUserPass().put(login,new Pair<String,Boolean>(passcode,true));
            response =  "CONNECTED" + "\n" + "version:"+acceptVer  + "\n" ;

        }else if(data.isUserLogedIn(login, passcode)){
            //if user already exsist's but no client connected to it send connect Frame, else send Error
            response = (data.getClietUserPass().get(login).getSecond() == false) ? "CONNECTED" + "\n" + "version:1.2"  + "\n" : Error(fullFrame, "User already logged in");
            
        }else{
            response = Error(fullFrame,"Wrong password");
        }
        
        return response;
    }

    @Override
    public String send(Map<String, String> headersSend, String FrameBody, int connectionId) {
        String destinationTopic = headersSend.get("destination");
        String Error = "SEND" +"\n" +"destination:".concat(destinationTopic) +"\n".concat(FrameBody);
        String response = "";

        if(data.isClientSubToTopic(destinationTopic,connectionId)){
            // we should send the message for every subscriber
            response = "sendMsgToAll" ;
        }else{
            //Return ERROR
            response = Error(Error, "You Not Subscribe To This Channel");
        }
        

        return response;
    }

    @Override
    public String subscribe(Map<String, String> headersSubscripe,int connectionId) {
        //check if user to topic that already subscribed in and maybe send error

        String destination = headersSubscripe.get("destination");
        String id  = headersSubscripe.get("id");
        //new topic
        String command = "RECEIPT";
        String headers  = "receipt-id:".concat(id);
        String body = "";
        String frame =  command + "\n" + headers + "\n" + body + "\n";
        //the topic not created in the past so the Cllient create it and add himself.
        if(data.getTopics().get(destination) == null){
            data.getTopics().compute(destination, (k, v) -> {
                ConcurrentLinkedQueue<Pair<String,Integer>> queue = new ConcurrentLinkedQueue<>();
                queue.add(new Pair<String,Integer>(id,connectionId));
                return queue;
            });
            
        }else if(data.getTopics().get(destination) != null && !data.getTopics().get(destination).contains(id) ){
            data.getTopics().get(destination).add(new Pair<String,Integer>(id,connectionId));

        }else{  
            body =  "you already subscribed to this channel!!! ";
            headers = "SUBSCRIBE" +"\n" +"destination:".concat(destination) +"\n".concat(id);
            return Error(headers, body);
        }
        return frame;
    }

    @Override
    public String unsubscribe(Map<String, String> headersUnsubscribe, int connectionId ,String frame) {
        String id  = headersUnsubscribe.get("id");
        String receipt = headersUnsubscribe.get("receipt");
        if(Data.removeSubscription(id,connectionId)){
            return Receipt(receipt);
        }else{
           return Error(frame, "There is No Subscription belong to his id");
        }
        


    }

    @Override
    public String disconnect(Map<String, String> headersDisconnect, int connectionId) {
        //delete all sunscreption that this client joined | done!
        // change the user status to fasle | done!
        //send recipt frame to the client and when the client recive the recipt he close he connection. 
        String receipt = headersDisconnect.get("receipt");
        Data.removeAllClientSubscribtion(connectionId);
        Data.changeUserStatus(UserStatus);
        //check when send the error if i disconected the connection in the right way!!!?
        return Receipt(receipt);
    }
    
    @Override
    public String Error(String header, String body) {
        String command = "ERROR";
        String hdr = "message: " + body;
        String bdy  = "The message:".concat("------------" + "\n").concat(header).concat("\n" + "------------");
        String StompErrorFrame = command + "\n" + hdr + "\n\n"+ bdy + "\n" ;
        //change user status wen send error cuase we close the connection
        Data.changeUserStatus(UserStatus);
        return StompErrorFrame;
    }

    @Override
    public String MESSAGE(String destinationTopic, String FrameBody, String subId) {
         String command = "MESSAGE";
         StringBuilder headerBuilder = new StringBuilder();
         headerBuilder.append("subscription:" + subId+'\n');
         headerBuilder.append("message - id:" + generateMessageId()+'\n');
         headerBuilder.append("destination:" + destinationTopic+'\n');
        return command + "\n" + headerBuilder.toString() + "\n\n" + FrameBody + "\n";
    }

    public static String generateMessageId(){
        long time = System.currentTimeMillis() /1000;
        int random = RANDOM.nextInt();
        return time + "" + random;
    }
     


    @Override
    public String Receipt(String receiptId) {
        String command = "RECEIPT";
        String header = "receipt-id:" + receiptId;

        return command + "\n" + header + "\n\n" + "" + "\n";
    }




    
    
}
