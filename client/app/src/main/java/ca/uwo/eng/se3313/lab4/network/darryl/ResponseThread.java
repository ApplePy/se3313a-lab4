package ca.uwo.eng.se3313.lab4.network.darryl;

import android.util.Log;

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;

import org.joda.time.DateTime;

import java.io.BufferedInputStream;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Date;

import ca.uwo.eng.se3313.lab4.network.GsonUtils;
import ca.uwo.eng.se3313.lab4.network.INetworkingConnection.OnErrorSend;
import ca.uwo.eng.se3313.lab4.network.response.ResponseVisitor;
import ca.uwo.eng.se3313.lab4.network.response.ServerError;

/**
 * Created by Darryl on 2016-12-11.
 */

public class ResponseThread extends Thread {

    private Socket sock;
    private ResponseVisitor visitor;
    private OnErrorSend error;


    /** ResponseThread Constructor
     *
     * @param socket        The socket to listen on.
     * @param visitorClass  The visitor class to be called when a response comes in.
     */
    public ResponseThread(Socket socket, ResponseVisitor visitorClass, OnErrorSend errorCb) {
        sock = socket;
        visitor = visitorClass;
        error = errorCb;
    }


    /** The main function of the thread.
     *
     */
    public void run() {
        try {
            // Get input stream
            Log.d("ReadThread", "Thread beginning processing.");
            BufferedInputStream bufIn = new BufferedInputStream(sock.getInputStream());

            // Loop through input stream and get data
            while (true) {

                try {
                    // Extract content
                    byte[] contentBuffer = new byte[2048];
                    String content;
                    int read = bufIn.read(contentBuffer);
                    content = new String(contentBuffer, 0, read);


                    // Visit contents if there is any data
                    if (content.length() > 0) {
                        Log.d("ReadThread", "bufContents: " + content);

                        try {
                            visitor.visit(content);
                        }
                        // A server error was thrown and the visitor thing didn't work.
                        catch (IllegalStateException e) {
                            JsonObject result = GsonUtils.buildGson().fromJson(content, JsonObject.class);
                            String message = result.getAsJsonObject("object").get("message").getAsString();
                            //DateTime time = DateTime.parse(result.getAsJsonObject("object").get("datetime").getAsString());
                            int code = Integer.parseInt(result.getAsJsonObject("object").get("code").getAsString());

                            visitor.visitError(new ServerError(DateTime.now(), code, message));
                        }
                    }
                } catch (SocketTimeoutException e) {
                    // Read timed out, don't worry about it.
                }
            }
        } catch (Exception e) {
            // Bad stuff happened. Tell someone!
            Log.e("ReadThread", "Exeception thrown: " + e.getMessage());
            error.onError(e);
        }
    }
}
