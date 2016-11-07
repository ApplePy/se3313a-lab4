package ca.uwo.eng.se3313.lab4.network.request;

import android.support.annotation.NonNull;

import org.joda.time.DateTime;

import ca.uwo.eng.se3313.lab4.network.AbstractNetworkInstance;

/**
 * Used when requesting to Login, there is very little payload.
 */
public class LoginRequest extends AbstractNetworkInstance {

    /**
     * Creates a new instance with `name`
     * @param username Who is trying to Login, set to be {@link #getSender()}
     */
    public LoginRequest(@NonNull final String username) {
        super(DateTime.now(), username);
    }
}
