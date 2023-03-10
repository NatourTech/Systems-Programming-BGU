package bgu.spl.net.api;

import java.util.Map;

public interface Commands {
    String connect(Map<String, String>  headersConnect,int connectionId,String fullFrame);
    String send(Map<String, String>  headersSend,String FrameBody,int connectionId);
    String subscribe(Map<String, String>  headersSubscripe ,int connectionId);
    String unsubscribe(Map<String, String>  headersUnsubscribe , int connectionId, String frame);
    String disconnect(Map<String, String>  headersDisconnect, int connectionId);
    String Error(String header, String body);
    String MESSAGE(String destinationTopic,String FrameBody,String id);
    String Receipt(String receiptId);
}
