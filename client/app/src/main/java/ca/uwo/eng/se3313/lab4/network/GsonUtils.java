package ca.uwo.eng.se3313.lab4.network;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonParseException;
import com.google.gson.JsonPrimitive;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;

import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;
import org.joda.time.format.DateTimeFormatterBuilder;
import org.joda.time.format.ISODateTimeFormat;

import java.lang.reflect.Type;

/**
 * Created by kevin on 2016-11-02.
 */

public abstract class GsonUtils {

    private GsonUtils() {
        // no construction allowed
    }

    /**
     * Helper factory method to produce {@link Gson} instances with the required adapters.
     * @return
     */
    public static Gson buildGson() {
        GsonBuilder builder = new GsonBuilder();

        builder.registerTypeAdapter(ErrorCode.class, new GsonErrorCodeHandler());
        builder.registerTypeAdapter(DateTime.class, new DateTimeTypeConverter());

        return builder.create();
    }

    /**
     * Converts joda.time.DateTime instances to {@link String}s and back.
     */
    private static class DateTimeTypeConverter implements JsonSerializer<DateTime>, JsonDeserializer<DateTime> {

        private static final DateTimeFormatter DATETIME_FORMAT =
                DateTimeFormat.forPattern("yyyy-MM-dd HH:mm:ss.SSSSSS Z");


        // No need for an InstanceCreator since DateTime provides a no-args constructor
        @Override
        public JsonElement serialize(DateTime src, Type srcType, JsonSerializationContext context) {
            return new JsonPrimitive(src.toString(DATETIME_FORMAT));
        }

        @Override
        public DateTime deserialize(JsonElement json, Type type, JsonDeserializationContext context)
                throws JsonParseException {
            final String dateString = json.getAsString();

            return DATETIME_FORMAT.parseDateTime(dateString);
        }
    }

    /**
     * Allows Gson to parse and serialize {@link ErrorCode} instances.
     */
    private static class GsonErrorCodeHandler implements JsonDeserializer<ErrorCode>, JsonSerializer<ErrorCode>  {
        @Override
        public ErrorCode deserialize(JsonElement json, Type typeOfT, JsonDeserializationContext context) throws JsonParseException {
            int code = json.getAsInt();

            return ErrorCode.fromCCode(code);
        }

        @Override
        public JsonElement serialize(ErrorCode src, Type typeOfSrc, JsonSerializationContext context) {
            return context.serialize(src.CODE, int.class);
        }
    }
}
