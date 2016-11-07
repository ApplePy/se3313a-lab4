package ca.uwo.eng.se3313.lab4.network.response;

import android.support.annotation.NonNull;

import com.google.gson.annotations.SerializedName;

import org.joda.time.DateTime;

import ca.uwo.eng.se3313.lab4.network.AbstractNetworkInstance;

/**
 * Contents of a Message sent to users connected to the server
 */
public class MessageResponse extends AbstractNetworkInstance {

    /**
     * The message payload.
     */
    @SerializedName("content")
    private final String mContent;

    /**
     * Who sent the message, {@link #getSender()} will always be the server.
     */
    @SerializedName("originator")
    private final String mOriginator;

    /**
     * Create a new message
     * @param time When the message was received
     * @param originator Who sent the message
     * @param sender Who sent this message to us (usually the server)
     * @param content Payload of the message
     */
    public MessageResponse(@NonNull DateTime time, @NonNull String originator,
                           @NonNull String sender, @NonNull String content) {
        super(time, sender);

        mContent = content;
        mOriginator = originator;
    }

    /**
     * Get the message payload.
     * @return Non-{@code null} message contents
     */
    public String getContent() {
        return mContent;
    }


    /**
     * Get the originating sender
     * @return username of the original sender
     */
    public String getOriginator() {
        return mOriginator;
    }
}
