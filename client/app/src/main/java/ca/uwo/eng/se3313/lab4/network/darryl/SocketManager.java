package ca.uwo.eng.se3313.lab4.network.darryl;

import android.os.AsyncTask;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.util.concurrent.locks.ReentrantLock;

import ca.uwo.eng.se3313.lab4.network.INetworkInstance;
import ca.uwo.eng.se3313.lab4.network.INetworkingConnection;
import ca.uwo.eng.se3313.lab4.network.response.ResponseVisitor;
import ca.uwo.eng.se3313.lab4.network.darryl.SocketConnect;
import ca.uwo.eng.se3313.lab4.network.darryl.SocketConnect.*;

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
    private ResponseThread readThread;

    /** Socket lock for sending
     */
    private ReentrantLock lock = new ReentrantLock();


    /** Constructor for SocketManager.
     *
     * @param visit     The visitor object to process the host responses.
     */
    public SocketManager(ResponseVisitor visit, String host, int port, SocketConnectError connectError, SocketConnectError connectionLost, SocketConnectSuccess connectSuccess) {

        // Set up private variables
        visitor = visit;

        // Connect the socket
        new SocketConnect(
                host,
                port,
                connectError,
                (Socket sock) -> {
                    // NOTE: Don't need to store returned socket.

                    // Start up listening thread
                    readThread = new ResponseThread(sock, visitor, (Throwable e) -> {
                        // TODO: Do something if a socket falls apart. #send expects the socket to work.
                        // This is just a stop-gap.
                        Log.d("SocketManager", "Connection lost.");
                        connectionLost.onError(e);
                    });
                    readThread.start();

                    // Call callback
                    connectSuccess.onSuccess(sock);
                }
        ).execute(sock);
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

        // Sanity check - ensure socket is connected.
        if (!sock.isConnected())
            errorHandler.onError(new SocketException("Socket is not connected."));


        /** This AsyncTask will send data out over interface.
         */
        AsyncTask<T, Object, T> task = new AsyncTask<T, Object, T>() {
            boolean success = false;
            IOException except = null;

            @Override
            protected T doInBackground(T... params) {
                try {
                    lock.lock();
                    // Get output stream
                    OutputStream out = sock.getOutputStream();

                    // Convert object to bytes and send
                    out.write(params[0].toJson().toString().getBytes());
                    out.flush();

                    success = true;
                    return params[0];
                } catch (IOException e) {
                    success = false;
                    except = e;
                    return null;
                } finally {
                    lock.unlock();
                }
            }

            @Override
            protected void onPostExecute(T o) {
                // If it was successful and there's a success handler, call it
                if(success && successHandler != null)
                    successHandler.onSuccess(myThis, o);

                // Call error callback if something went wrong.
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
        // Close up reading thread and the socket.
        readThread.join();
        sock.close();
    }
}
