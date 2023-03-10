package bgu.spl.net.srv;
import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.srv.ConnectionHandler;
public class ConnectionsImp<T> implements Connections<T> {
    //Constructor 
    int connectionId;
    AtomicInteger conectionId=new AtomicInteger(0);
    //<key:value>
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> clientToHandler = new ConcurrentHashMap<>();


    public ConnectionsImp(){
        // this.connectionId =  conectionId.get();;
        // conectionId.incrementAndGet();
    }

    public int addConnections(ConnectionHandler<T> ClientHandler){

         this.clientToHandler.put(conectionId.get(),ClientHandler);
         int id = conectionId.get();
        //Increment id for new future conection 
        conectionId.incrementAndGet();
        System.out.println("New Conection Id Added");
         return id;
     }



    @Override
    public boolean send(int connectionId, T msg) {
        if(this.clientToHandler.get(connectionId) != null){
            //To prevent user's in an client to right massage's at the same time.
            String msgg = (String) msg;
            synchronized(this.clientToHandler.get(connectionId)){
                this.clientToHandler.get(connectionId).send(msg);
                //Check if the Response message to client is an Error 
                if(msgg.startsWith("ERROR")){
                    try{
                        //Close The Connection After Sending Error.
                        this.clientToHandler.get(connectionId).close();
                        //Remove the Connection handler because its closed
                        disconnect(connectionId);
                    } catch(IOException e){
                        // error catched whil-e closing connection

                        ///what to do with the user status !!!!!!///
                    }
                }
            }

            return true;
        }

        return false;
    }

    @Override
    //send message T to cliet's subscribed to channel 
    public void send(String channel, T msg) {
        

    }

    @Override
    public void disconnect(int connectionId) {
        //Remove Connction 
        
        this.clientToHandler.remove(connectionId);
        
    }
    
}
