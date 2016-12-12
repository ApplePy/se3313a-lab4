package ca.uwo.eng.se3313.lab4.network.darryl;

import android.os.AsyncTask;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;

/**
 * Created by Darryl on 2016-12-11.
 */

public class SocketConnect extends AsyncTask<Socket, Void, Socket> {
    private IOException except = null;

    private String host;
    private int port;

    private SocketConnectError errorCb;
    private SocketConnectSuccess successCb;


    /** Interface to implement when a socket cannot connect.
     */
    public interface SocketConnectError {

        void onError(Throwable error);
    }

    /** Interface to implmement when a socket connects successfully.
     */
    public interface SocketConnectSuccess {
        void onSuccess(Socket connectedSocket);
    }


    /** SocketConnect constructor. Will connect a socket to the given hostname.
     *
     * @param host  The hostname to connect sockets to.
     * @param port  The port to connect sockets to.
     */
    public SocketConnect(String host, int port, SocketConnectError errorSend, SocketConnectSuccess successfulSend) {
        this.host = host;
        this.port = port;
        errorCb = errorSend;
        successCb = successfulSend;
    }


    /**
     * Override this method to perform a computation on a background thread. The
     * specified parameters are the parameters passed to {@link #execute}
     * by the caller of this task.
     * <p>
     * This method can call {@link #publishProgress} to publish updates
     * on the UI thread.
     *
     * @param params The parameters of the task.
     * @return A result, defined by the subclass of this task.
     * @see #onPreExecute()
     * @see #onPostExecute
     * @see #publishProgress
     */
    @Override
    protected Socket doInBackground(Socket... params) {
        Log.d("SocketConnect", "Starting connection process.");
        try
        {
            // Resolve hostname
            Log.d("SocketManager", "Resolving host.");
            InetAddress ihost = InetAddress.getByName(host);

            // Connect socket to host
            Log.d("SocketManager", "Starting socket connection.");
            Socket sock = params[0];
            sock.connect(new InetSocketAddress(ihost, port), 2000); // 2000ms to connect or fails
            sock.setSoTimeout(5000);                                // 5000ms timeout on reads

            return sock;
        } catch (IOException e) {
            except = e;
            return null;
        }
    }


    /**
     * <p>Runs on the UI thread after {@link #doInBackground}. The
     * specified result is the value returned by {@link #doInBackground}.</p>
     *
     * <p>This method won't be invoked if the task was cancelled.</p>
     *
     * @param result The result of the operation computed by {@link #doInBackground}.
     *
     * @see #onPreExecute
     * @see #doInBackground
     * @see #onCancelled(Object)
     */
    @Override
    protected void onPostExecute(@Nullable Socket result) {

        // If socket connection failed.
        if (result == null){
            Log.e("SocketManager", "Open socket failed.");
            errorCb.onError(except);
        }

        // Socket connected successful.
        else {
            Log.d("SocketManager", "Open socket success.");
            successCb.onSuccess(result);
        }
    }
}
