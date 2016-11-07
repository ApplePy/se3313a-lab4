package ca.uwo.eng.se3313.lab4.network.response;

import android.support.annotation.NonNull;

import com.google.gson.JsonObject;

import ca.uwo.eng.se3313.lab4.network.ErrorCode;

/**
 * Implementation of the <a href="https://en.wikipedia.org/wiki/Visitor_pattern">Visitor Pattern</a>
 * which allows for simple breakdown of different messages all coming from a single source.
 * <p>
 *
 * {@link AbstractResponseVisitor} provides a better starting point if able to utilize it.
 *
 * @see AbstractResponseVisitor
 */
public interface ResponseVisitor{

    /**
     * Visits a json object, calling the appropriate response method.
     * @param json Sequence of characters (a {@link String}, likely) to be parsed from a source.
     */
    void visit(@NonNull final CharSequence json);

    /**
     * Called when a {@link LoginResponse} instance is recieved by {@link #visit(CharSequence)}.
     * @param login The response received
     */
    void visitLogin(@NonNull final LoginResponse login);

    /**
     * Called if a {@link MessageResponse} is received.
     * @param message The message received
     */
    void visitMessage(@NonNull final MessageResponse message);

    /**
     * Called if the server returns an error.
     *
     * @param error The error message.
     */
    void visitError(@NonNull final ServerError error);

    /**
     * Called if there was an internal error while parsing
     * @param code The error code expected.
     * @param message Details of the error
     */
    void error(@NonNull final ErrorCode code, final String message);

}
