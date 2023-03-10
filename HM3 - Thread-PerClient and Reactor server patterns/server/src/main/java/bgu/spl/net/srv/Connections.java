package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {
    int addConnections(ConnectionHandler<T> connections);
    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);
}
