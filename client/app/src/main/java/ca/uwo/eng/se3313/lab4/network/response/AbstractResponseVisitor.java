package ca.uwo.eng.se3313.lab4.network.response;

import android.support.annotation.NonNull;

import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;

import ca.uwo.eng.se3313.lab4.network.AbstractNetworkInstance;
import ca.uwo.eng.se3313.lab4.network.GsonUtils;
import ca.uwo.eng.se3313.lab4.network.INetworkInstance;

/**
 * Base implementation of the {@link ResponseVisitor} class. This will accept JsonObject
 * instances and call the appropriate visit* method.
 *
 */
public abstract class AbstractResponseVisitor implements ResponseVisitor {

    protected final Gson mGson;

    public AbstractResponseVisitor() {
        mGson = GsonUtils.buildGson();
    }

    @Override
    public void visit(@NonNull final CharSequence json) {
        final JsonObject obj = mGson.fromJson(json.toString(), JsonObject.class);

        final String typeStr = obj.get(INetworkInstance.PROPERTY_TYPE).getAsString();

        @SuppressWarnings("unchecked")
        Class<? extends INetworkInstance> type;
        try {
            type = (Class<? extends INetworkInstance>)Class.forName(typeStr);
        } catch (ClassNotFoundException cnfe) {
            throw new IllegalArgumentException("Invalid formatted json object, type was invalid " +
                    "(type=" + typeStr + ").", cnfe);
        } catch (ClassCastException cce) {
            throw new IllegalArgumentException("Invalid type specified, does not inherit from " +
                    AbstractNetworkInstance.class.getName() + " (type=" + typeStr + ").", cce);
        }

        final JsonElement jsonObject = obj.get(INetworkInstance.PROPERTY_OBJECT);

        // Annoying "dynamic cast"-esque checks
        if (type.equals(LoginResponse.class)) {
            this.visitLogin(mGson.fromJson(jsonObject, LoginResponse.class));
        } else if (type.equals(MessageResponse.class)) {
            this.visitMessage(mGson.fromJson(jsonObject, MessageResponse.class));
        } else {
            throw new IllegalStateException("Could not visit json argument: " + obj);
        }
    }
}
