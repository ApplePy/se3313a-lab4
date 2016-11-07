package ca.uwo.eng.se3313.lab4.network;

import android.support.annotation.NonNull;

import com.google.gson.Gson;
import com.google.gson.JsonObject;

import org.joda.time.DateTime;

import java.lang.reflect.Type;

/**
 * Used to denote an instance sent across to the server or returned from the server.
 */
public interface INetworkInstance {
    // marker interface

    /**
     * The tag for how the type is stored.
     */
    public static final String PROPERTY_TYPE = "type";

    /**
     * Tag for when the time is specified.
     */
    public static final String PROPERTY_DATETIME = "datetime";

    /**
     * Property with the object storing the Json from {@link Gson#toJson(Object, Type)}
     */
    public static final String PROPERTY_OBJECT = "object";

    /**
     * Special "sender" that is the Server rather than a user.
     */
    public static final String SENDER_SERVER = "@server";

    /**
     * Time the message was created.
     * @return Non-{@code null} time when the message was sent.
     */
    @NonNull DateTime getDateTime();

    /**
     * Get the username that sent the message
     * @return Non-{@code null} useranme
     */
    @NonNull String getSender();

    /**
     * Converts an instance of {@link INetworkInstance} into Json.
     * @return JsonObject representation.
     */
    @NonNull JsonObject toJson();
}
