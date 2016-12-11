package ca.uwo.eng.se3313.lab4;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.util.regex.Pattern;

import ca.uwo.eng.se3313.lab4.network.INetworkingConnection;
import ca.uwo.eng.se3313.lab4.network.SocketManager;
import ca.uwo.eng.se3313.lab4.network.request.LoginRequest;
import ca.uwo.eng.se3313.lab4.network.response.ResponseVisitor;

import static com.google.common.base.Preconditions.checkNotNull;

/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link OnInteractionListener} interface
 * to handle interaction events.
 * Use the {@link LoginFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class LoginFragment extends Fragment {

    /**
     * Tag for {@link LoginFragment}
     */
    public static final String TAG = LoginFragment.class.getName();

    private View mLoginFormView;

    /**
     * Reference to the Activity or Fragment that created this instance.
     */
    private OnInteractionListener mListener;

    // TODO SE3313
    // Add any fields you want for state below this
    private String address;
    private int port;
    private String username;

    private TextView address_field;
    private TextView port_field;
    private TextView username_field;
    private ProgressBar progress_view;
    private TextView error_text;
    private Button signin_button;


    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        this.mLoginFormView = view.findViewById(R.id.login_form);

        // Get view references
        address_field = (TextView) view.findViewById(R.id.address);
        port_field = (TextView) view.findViewById(R.id.port);
        username_field = (TextView) view.findViewById(R.id.username);
        progress_view = (ProgressBar) view.findViewById(R.id.login_progress);
        error_text = (TextView) view.findViewById(R.id.error_text);
        signin_button = (Button) view.findViewById(R.id.sign_in_button);

        // Set default text
        address_field.setText(R.string.default_server_address);
        address = getText(R.string.default_server_address).toString();
        port_field.setText(R.string.default_port);
        port = Integer.parseInt(getText(R.string.default_port).toString());
        username_field.setText(R.string.default_username);
        username = getText(R.string.default_username).toString();

        // Set the changed listeners so that local variables are always kept up to date
        address_field.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                Log.d("ListenerAddress", s.toString());
                address = s.toString();
            }

            @Override
            public void afterTextChanged(Editable s) {}
        });

        port_field.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // Don't let illegal values happen!
                // TODO: Handle situation where the text becomes empty better.
                if(s.length() == 0) {
                    s = "1";
                }
                Log.d("ListenerPort", s.toString());
                port = Integer.parseInt(s.toString());
            }

            @Override
            public void afterTextChanged(Editable s) {}
        });

        username_field.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                Log.d("ListenerUsername", s.toString());
                username = s.toString();
            }

            @Override
            public void afterTextChanged(Editable s) {}
        });

        signin_button.setOnClickListener((View v) -> {
            Log.d("LoginFragment", address);
            Log.d("LoginFragment", Integer.toString(port));
            Log.d("LoginFragment", username);

            // Validate information
            if (isValidHostname(address_field.getText().toString()) &&
                    username.length() > 2 &&
                    port > 0 && port <= 65535) {
                // Wipe old error messages
                error_text.setText("");

                // Create login request
                Log.d("Login: ", "Valid");
                LoginRequest loginObj = new LoginRequest(username);

                // Start login
                mListener.login(loginObj, address, port, error_text);

            } else {
                Log.d("Login: ", "Invalid");
                error_text.setText("Invalid host, port, or username.");
            }
        });
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();

        mLoginFormView.animate().cancel();
        this.mLoginFormView = null;

        // SE3313 Set all of your view references to null here
        address_field = null;
        port_field = null;
        username_field = null;
        progress_view = null;
    }




    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     */
    public interface OnInteractionListener {

       // TODO SE3313 Add any interactions you expect the MainActivity to have
        void login(LoginRequest req, String host, int port, TextView errors);

    }

    // ------ DO NOT MODIFY BELOW THIS ------
    // The following are all used to create UI components, you'll want to use the functions but do
    // not change them.

    public LoginFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment LoginFragment.
     */
    public static LoginFragment newInstance(@Nullable Bundle bundle) {
        final LoginFragment fragment = new LoginFragment();
        fragment.setArguments(bundle);

        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setHasOptionsMenu(false);
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);

        // Assign the listener
        try {
            mListener = (OnInteractionListener) context;
        } catch (ClassCastException e) {
            throw new ClassCastException(context + " must implement OnInteractionListener");
        }

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_login, container, false);
    }

    private static final Pattern P_HOSTNAME = Pattern.compile("^(?:(?:\\w|\\w[\\w\\-]*\\w)\\.)*(?:\\w|\\w[\\w\\-]*\\w)$",
            Pattern.CASE_INSENSITIVE);
    private static final Pattern P_IPADDR = Pattern.compile("^(?:(?:\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])\\.){3}(?:\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])$");

    /**
     * Checks if a hostname is valid
     * @param address Address to match against
     * @return {@code true} if the host is valid
     *
     * @see <a href="http://stackoverflow.com/a/106223/1748595">Original Source</a>
     */
    private boolean isValidHostname(@NonNull final String address) {
        return P_HOSTNAME.matcher(address).matches() || P_IPADDR.matcher(address).matches();
    }
}
