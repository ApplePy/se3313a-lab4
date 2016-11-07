package ca.uwo.eng.se3313.lab4.network;

import android.os.AsyncTask;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import java.net.InetAddress;

import ca.uwo.eng.se3313.lab4.network.response.ResponseVisitor;

/**
 * Created by kevin on 2016-11-02.
 */
public interface INetworkingConnection extends AutoCloseable {

    /**
     * Get the remote {@link InetAddress} of a connection
     *
     * @return Non-{@code null} address instance.
     */
    @NonNull
    String getAddress();

    /**
     * Get the port number for this connection
     * @return Port value
     */
    int getPort();

    /**
     * Get the current response handler being used.
     * @return Handler instance
     */
    @NonNull ResponseVisitor getResponseHandler();

    /**
     * Creates an {@link AsyncTask} to send data to the server and starts it immediately.
     *
     * @param message Message to send to the server
     * @param errorHandler Callback if an error occurs.
     * @param successHandler Callback if the operation was successful, can be {@code null}.
     * @return The currently executing {@link AsyncTask}.
     */
    <T extends INetworkInstance>
    AsyncTask<T, ?, ?> send(@NonNull T message,
                            @NonNull OnErrorSend errorHandler,
                            @Nullable OnSuccessfulSend<T> successHandler);

    /**
     * Delegate method to {@link #send(INetworkInstance, OnErrorSend, OnSuccessfulSend)} that
     * passes {@code null} to for the {@link OnSuccessfulSend}.
     *
     * @param message Message to send to the server
     * @param errorHandler Callback if an error occurs.
     * @return The currently executing {@link AsyncTask}.
     *
     * @see #send(INetworkInstance, OnErrorSend, OnSuccessfulSend)
     */
    <T extends INetworkInstance> AsyncTask<T, ?, ?>
        send(@NonNull T message,
             @NonNull OnErrorSend errorHandler);

    /**
     * Used for a callback when a message is successfully sent via
     * {@link #send(INetworkInstance, OnErrorSend, OnSuccessfulSend)}.
     *
     * @param <T> The subtype of the message successfully sent.
     */
    public interface OnSuccessfulSend<T extends INetworkInstance> {

        /**
         * Called passing the {@link INetworkingConnection} instance and the message passed. This is
         * expected to run on the UI thread.
         *
         * @param connection Instance of the connection
         * @param message The instance sent
         */
        @UiThread
        public void onSuccess(INetworkingConnection connection, @NonNull T message);
    }

    /**
     * Callback for if an error happens when executing
     * {@link #send(INetworkInstance, OnErrorSend, OnSuccessfulSend)}.
     */
    public interface OnErrorSend {

        /**
         * Called when an error occurs. This is expected to run on the UI thread.
         * @param error Error thrown
         */
        @UiThread
        public void onError(@NonNull Throwable error);
    }
}
