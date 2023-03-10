package bgu.spl.net.srv;
import java.io.IOException;

import bgu.spl.net.srv.ConnectionHandler;
public class ConnectionHandlerImp<T> implements ConnectionHandler<T> {

    @Override
    public void send(T msg) {
        // TODO Auto-generated method stub


        
    }

    @Override
    public void close() throws IOException {
        System.out.println("Hi, From Cloasble in connectionHandler");
    }

   
}
