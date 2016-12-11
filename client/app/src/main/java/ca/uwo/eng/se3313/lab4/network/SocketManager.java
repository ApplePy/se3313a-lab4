package ca.uwo.eng.se3313.lab4.network;

import android.os.AsyncTask;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import ca.uwo.eng.se3313.lab4.network.response.ResponseVisitor;

/**
 * Created by Darryl on 2016-12-11.
 */

public class SocketManager implements INetworkingConnection {

    /** The persistent socket to the server.
     */
    private Socket sock = new Socket();

    /** The visitor class for server responses.
     */
    final private ResponseVisitor visitor;

    /** Thread for reading server data.
     */
    private Thread readThread;

    /** Control variable to signal close of socket.
     */
    private boolean kill = false;


    /** Constructor for SocketManager.
     *
     * @param visit     The visitor object to process the host responses.
     */
    public SocketManager(ResponseVisitor visit) {
        // Assign visitor
        visitor = visit;
    }

    /** Socket maker for SocketManager. Uses string hostname instead.
     *
     * @param host      The host to connect to.
     * @param port      The host port to connect to.
     */
    public void open(String host, int port) {

        // Start lookup
        new AsyncTask<Void, Void, InetAddress>() {
            @Override
            protected InetAddress doInBackground(Void... params) {
                try {
                    return InetAddress.getByName(host);
                } catch (UnknownHostException e) {
                    return null;
                }
            }

            @Override
            protected void onPostExecute(@Nullable InetAddress inetAddress) {
                if (inetAddress == null) {
                    Log.e("SocketManager", "Unknown host.");
                } else {
                    // TODO: What to do when the socket doesn't open???
                    open(inetAddress, port);
                }
            }
        }.execute();
    }

    /** Socket constructor for SocketManager. Creates socket and starts long-running thread for
     * receiving data.
     *
     * @param host The InetAddress of the remote server.
     * @param port The port for the remote server.
     */
    public void open(InetAddress host, int port) {

        // Don't mess with an already-open socket or a closing SocketManager.
        if (sock.isConnected() || kill)
            return;

        // TODO: What to do when the socket doesn't open???

        // Thread for reading input from socket
        readThread = new Thread() {
            public void run() {
                try {
                    while (!sock.isConnected());    // BUSY WAIT! BAD! TODO: Fix busy-wait

                    // Get input from server, and visit
                    BufferedInputStream bufIn = new BufferedInputStream(sock.getInputStream());
                    while (!kill) {
                        // Visit contents
                        visitor.visit(bufIn.toString());
                    }
                } catch (IOException e) {
                    // Respawn
                    readThread.start();
                }
            }
        };

        // AsyncTask for getting socket open.
        new AsyncTask<Void, Void, Socket>() {
            IOException except = null;

            @Override
            protected Socket doInBackground(Void... params) {
                try
                {
                    Socket sock = new Socket(host, port);
                    sock.setSoTimeout(5000);    // 5000ms timeout on reads
                    return sock;
                } catch (IOException e) {
                    except = e;
                    return null;
                }
            }

            @Override
            protected void onPostExecute(@Nullable Socket openSock) {
                if (openSock == null){
                    Log.e("SocketManager", "Open socket failed.");
                    // TODO: Handle error here
                } else {
                    // Setup thread and start reader.
                    sock = openSock;
                    readThread.start();
                }
            }
        }.execute();
    }


    @NonNull
    @Override
    public String getAddress() {
        return sock.getInetAddress().getHostAddress();
    }

    @Override
    public int getPort() {
        return sock.getPort();
    }

    @NonNull
    @Override
    public ResponseVisitor getResponseHandler() {
        return visitor;
    }

    @Override
    public <T extends INetworkInstance> AsyncTask<T, ?, ?> send(@NonNull T message, @NonNull OnErrorSend errorHandler, @Nullable OnSuccessfulSend<T> successHandler) {
        INetworkingConnection myThis = this;

        AsyncTask<T, Object, T> task = new AsyncTask<T, Object, T>() {
            boolean success = false;
            IOException except = null;

            @Override
            protected T doInBackground(T... params) {
                try {
                    // Get output stream
                    OutputStream out = sock.getOutputStream();

                    // Convert object to bytes and send
                    out.write(params[0].toJson().toString().getBytes());
                    out.flush();

                    success = true;
                    return params[0];
                } catch (IOException e) {
                    except = e;
                    return null;
                }
            }

            @Override
            protected void onPostExecute(T o) {
                if(success && successHandler != null)
                    successHandler.onSuccess(myThis, o);
                if (!success)
                    errorHandler.onError(except);
            }
        };

        return task.execute(message);
    }

    @Override
    public <T extends INetworkInstance> AsyncTask<T, ?, ?> send(@NonNull T message, @NonNull OnErrorSend errorHandler) {
        return send(message, errorHandler, null);
    }

    @Override
    public void close() throws Exception {
        kill = true;
        readThread.join();
        sock.close();
    }
}
