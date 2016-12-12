package ca.uwo.eng.se3313.lab4.network.darryl;

import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.util.Log;

import ca.uwo.eng.se3313.lab4.MainActivity;
import ca.uwo.eng.se3313.lab4.network.ErrorCode;
import ca.uwo.eng.se3313.lab4.network.response.AbstractResponseVisitor;
import ca.uwo.eng.se3313.lab4.network.response.LoginResponse;
import ca.uwo.eng.se3313.lab4.network.response.MessageResponse;
import ca.uwo.eng.se3313.lab4.network.response.ServerError;

/**
 * Created by Darryl on 2016-12-11.
 */

public class MyResponseVisitor extends AbstractResponseVisitor {
    private Handler appHandler;

    /** MyResponseVisitor constructor.
     *
     * @param app The app's handler loop to report actions to.
     */
    public MyResponseVisitor(Handler app) {
        appHandler = app;
    }

    /**
     * Called when a {@link LoginResponse} instance is recieved by {@link #visit(CharSequence)}.
     *
     * @param login The response received
     */
    @Override
    public void visitLogin(@NonNull LoginResponse login) {
        appHandler.sendMessage(Message.obtain(appHandler, MainActivity.DisplayLogin, new Object[] {login.getDateTime(), login.getJoiningUsername()}));
    }

    /**
     * Called if a {@link MessageResponse} is received.
     *
     * @param message The message received
     */
    @Override
    public void visitMessage(@NonNull MessageResponse message) {
        appHandler.sendMessage(Message.obtain(appHandler, MainActivity.DisplayMessage, new Object[] {message.getDateTime(), message.getOriginator(), message.getContent()}));
    }

    /**
     * Called if the server returns an error.
     *
     * @param error The error message.
     */
    @Override
    public void visitError(@NonNull ServerError error) {
        appHandler.sendMessage(Message.obtain(appHandler, MainActivity.DisplayError, error.getMessage()));
    }

    /**
     * Called if there was an internal error while parsing
     *
     * @param code    The error code expected.
     * @param message Details of the error
     */
    @Override
    public void error(@NonNull ErrorCode code, String message) {
        Log.e("MyResponseVisitor_error", message + code.name());
    }
}
