package ca.uwo.eng.se3313.lab4.network;

import android.support.annotation.NonNull;

import ca.uwo.eng.se3313.lab4.network.response.AbstractResponseVisitor;
import ca.uwo.eng.se3313.lab4.network.response.LoginResponse;
import ca.uwo.eng.se3313.lab4.network.response.MessageResponse;
import ca.uwo.eng.se3313.lab4.network.response.ServerError;

/**
 * Created by Darryl on 2016-12-11.
 */

public class MyResponseVisitor extends AbstractResponseVisitor {
    /**
     * Called when a {@link LoginResponse} instance is recieved by {@link #visit(CharSequence)}.
     *
     * @param login The response received
     */
    @Override
    public void visitLogin(@NonNull LoginResponse login) {

    }

    /**
     * Called if a {@link MessageResponse} is received.
     *
     * @param message The message received
     */
    @Override
    public void visitMessage(@NonNull MessageResponse message) {

    }

    /**
     * Called if the server returns an error.
     *
     * @param error The error message.
     */
    @Override
    public void visitError(@NonNull ServerError error) {

    }

    /**
     * Called if there was an internal error while parsing
     *
     * @param code    The error code expected.
     * @param message Details of the error
     */
    @Override
    public void error(@NonNull ErrorCode code, String message) {

    }
}
