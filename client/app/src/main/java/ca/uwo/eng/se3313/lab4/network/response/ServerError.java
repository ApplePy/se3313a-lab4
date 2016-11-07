package ca.uwo.eng.se3313.lab4.network.response;

import android.support.annotation.NonNull;

import com.google.gson.annotations.SerializedName;

import org.joda.time.DateTime;

import ca.uwo.eng.se3313.lab4.network.AbstractNetworkInstance;
import ca.uwo.eng.se3313.lab4.network.ErrorCode;
import ca.uwo.eng.se3313.lab4.network.INetworkInstance;

/**
 * The result when the server returns an error.
 */
public final class ServerError extends AbstractNetworkInstance {

    @SerializedName("code")
    private final ErrorCode mCode;

    @SerializedName("message")
    private final String mMessage;

    public ServerError(@NonNull DateTime dateTime, int code, String message) {
        super(dateTime, INetworkInstance.SENDER_SERVER);
        this.mCode = ErrorCode.fromCCode(code);
        this.mMessage = message;
    }

    public String getMessage() {
        return mMessage;
    }

    public ErrorCode getCode() {
        return mCode;
    }
}
