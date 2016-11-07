package ca.uwo.eng.se3313.lab4.network;

import android.support.annotation.NonNull;

import com.google.common.base.MoreObjects;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.annotations.SerializedName;

import org.joda.time.DateTime;


/**
 * Defines an object to be sent across the network interface
 */
public abstract class AbstractNetworkInstance implements INetworkInstance {

    /**
     * The time that a network instance is sent, for requests this is the client time,
     * for a server response this is the server time.
     */
    @SerializedName("datetime")
    private final DateTime mDateTime;

    /**
     * Who sent the message
     */
    @SerializedName("sender")
    private final String mSender;

    /**
     * Used for converting objects into Json
     */
    private final static transient Gson mGson = GsonUtils.buildGson();

    /**
     * Create a new instance of an {@link AbstractNetworkInstance}
     * @param time The time the message was created
     * @param username The sending username
     */
    protected AbstractNetworkInstance(@NonNull final DateTime time, @NonNull final String username) {
        this.mDateTime = time;
        this.mSender = username;
    }

    @Override
    public final @NonNull DateTime getDateTime() {
        return mDateTime;
    }

    @Override
    public final @NonNull String getSender() {
        return mSender;
    }

    @Override
    public final JsonObject toJson() {
        JsonObject obj = new JsonObject();

        final Class<? extends INetworkInstance> clazz = getClass();

        obj.addProperty(PROPERTY_TYPE, clazz.getName());
        obj.add(PROPERTY_OBJECT, mGson.toJsonTree(this));

        return obj;
    }

    @Override
    public String toString() {
        return MoreObjects.toStringHelper(getClass()).toString();
    }
}
