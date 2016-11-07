package ca.uwo.eng.se3313.lab4.network.request;

import android.support.annotation.NonNull;

import com.google.gson.annotations.SerializedName;

import org.joda.time.DateTime;

import ca.uwo.eng.se3313.lab4.network.AbstractNetworkInstance;

/**
 * Used to send messages
 */
public class MessageRequest extends AbstractNetworkInstance {

    /**
     * The payload of the message.
     */
    @SerializedName("content")
    private final String mContent;

    /**
     * Create a new message
     * @param time When the message was sent
     * @param username Sending username
     * @param content Message payload
     */
    public MessageRequest(@NonNull DateTime time, @NonNull String username, @NonNull String content) {
        super(time, username);

        mContent = content;
    }

    /**
     * Get the message contents.
     * @return Non-{@code null} message contents
     */
    public String getContent() {
        return mContent;
    }
}
