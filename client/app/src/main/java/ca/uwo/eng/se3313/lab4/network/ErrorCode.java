package ca.uwo.eng.se3313.lab4.network;

import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonParseException;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;

import java.lang.reflect.Type;

/**
 * Represents possible errors from the Server
 */
@SuppressWarnings("unused")
public enum ErrorCode {

    INVALID_REQUEST_FROM_CLIENT(50),

    /*!
     * The Username already exists on the server at this time.
     */
    USER_NAME_IN_USE(1),

    /*!
     * The User Name was invalid (e.g. empty).
     */
    INVALID_USER_NAME(2),

    MALFORMED_REQUEST_UNKNWN(200),

    MALFORMED_REQUEST_NO_TYPE(201),
    MALFORMED_REQUEST_UNKNOWN_TYPE(202),
    MALFORMED_REQUEST_BAD_OBJECT_DEF(203),
    MALFORMED_REQUEST_NO_OBJ(210);

    public final int CODE;

    ErrorCode(final int cCode) {
        this.CODE = cCode;
    }

    public static ErrorCode fromCCode(final int code) {
        for (ErrorCode ec : ErrorCode.values()) {
            if (ec.CODE == code) {
                return ec;
            }
        }

        throw new IllegalArgumentException("Code is not known, mCode=" + code);
    }
}
