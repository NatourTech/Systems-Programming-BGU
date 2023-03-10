package bgu.spl.net.impl.stomp;
import  bgu.spl.net.srv.Server;
import bgu.spl.net.api.StompMessagingProtocolImp;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.api.MessageEncoderDecoder;
public class StompServer {

    public static void main(String[] args) {
        // TODO: implement this
        String port  = args[0];

        if(args[1].equals("tpc")){
            Server.threadPerClient(
                Integer.parseInt(port), //port
                () -> new StompMessagingProtocolImp(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();

        }else if(args[1].equals("reactor")){
        
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    Integer.parseInt(port), //port
                    () -> new StompMessagingProtocolImp<>(), //protocol factory
                    LineMessageEncoderDecoder::new //message encoder decoder factory
            ).serve();
        }else{
            System.out.println("pleas put and valide arg's");
        }

    }
}
