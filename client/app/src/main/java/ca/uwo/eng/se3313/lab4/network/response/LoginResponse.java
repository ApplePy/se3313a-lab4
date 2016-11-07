package ca.uwo.eng.se3313.lab4.network.response;

import android.support.annotation.NonNull;

import com.google.gson.annotations.SerializedName;

import org.joda.time.DateTime;

import ca.uwo.eng.se3313.lab4.network.AbstractNetworkInstance;
import ca.uwo.eng.se3313.lab4.network.INetworkInstance;

/**
 * Passed when a user joins the server.
 */
public final class LoginResponse extends AbstractNetworkInstance {

    /**
     * The username that just joined.
     */
    @SerializedName("joiningUsername")
    private final String mJoiningUsername;

    LoginResponse(@NonNull final DateTime dateTime, @NonNull final String username) {
        super(dateTime, INetworkInstance.SENDER_SERVER);

        this.mJoiningUsername = username;
    }

    /**
     * The username that joined.
     * @return Joining username
     */
    public String getJoiningUsername() {
        return mJoiningUsername;
    }
}
