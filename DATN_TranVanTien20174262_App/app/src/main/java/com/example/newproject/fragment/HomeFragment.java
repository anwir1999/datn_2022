package com.example.newproject.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.example.newproject.MainActivity;
import com.example.newproject.R;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link HomeFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class HomeFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public HomeFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment HomeFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static HomeFragment newInstance(String param1, String param2) {
        HomeFragment fragment = new HomeFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }
    private Button btMess, btLock;
    private MainActivity mainActivity;
    public TextView textView;
    private SwipeRefreshLayout swipeRefreshLayout;
    private LinearLayout linearLayout;
    private RelativeLayout relativeLayout;
    private View view;
    private TextView tpDevice, latDevice, lonDevice, timeDevice, dateDevice;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        view = inflater.inflate(R.layout.fragment_home, container, false);
        btMess = (Button)view.findViewById(R.id.bt_mess);
        btLock = view.findViewById(R.id.bt_unlock);
        textView = view.findViewById(R.id.st_check);
        swipeRefreshLayout = view.findViewById(R.id.swipeRefreshLayout);
        relativeLayout = view.findViewById(R.id.relative1);
        tpDevice = view.findViewById(R.id.tpDevice);
        latDevice = view.findViewById(R.id.latDevice);
        lonDevice = view.findViewById(R.id.lonDevice);
        timeDevice = view.findViewById(R.id.timeDevice);
        dateDevice = view.findViewById(R.id.dateDevice);
        linearLayout = view.findViewById(R.id.linearLayout);

        swipeRefreshLayout.setOnRefreshListener(this);
        mainActivity = (MainActivity) getActivity();
        mainActivity.PostServiceAPI();
        btMess.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //mainActivity.publishMessage("hello");
                mainActivity.callServiceAPI();
                relativeLayout.setVisibility(View.VISIBLE);
                tpDevice.setText(mainActivity.getTp());
                latDevice.setText(mainActivity.getLatht());
                lonDevice.setText(mainActivity.getLonht());
                timeDevice.setText(mainActivity.getTimeht());
                dateDevice.setText(mainActivity.getDateht());
            }
        });
        btLock.setOnClickListener(new View.OnClickListener() {
            @SuppressLint("SetTextI18n")
            @Override
            public void onClick(View v) {
                if(mainActivity.getSt_check().equals("on"))
                {
                    mainActivity.publishMessageAtributes("{\"stCheck\":\"off\"}");
                    mainActivity.publishMessageHT("{\"stCheck\":\"off\"}");
                    mainActivity.setSt_check("off");
                    textView.setText("off");
                }
                else if(mainActivity.getSt_check().equals("off"))
                {
                    mainActivity.publishMessageAtributes("{\"stCheck\":\"on\",\"stpk\":\"off\",\"tp\":\"30\"}");
                    mainActivity.publishMessageHT("{\"stCheck\":\"on\",\"stpk\":\"off\",\"tp\":\"30\"}");
                    mainActivity.setSt_check("on");
                    textView.setText("on");
                }
            }
        });
        return view;
    }
    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
    }
    @Override
    public void onRefresh() {
        mainActivity.callServiceAPI();
        if(mainActivity.getSt_check() == null)
        {
        }
        else
        {
            linearLayout.setVisibility(View.VISIBLE);
            textView.setText(mainActivity.getSt_check());
        }
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {

                swipeRefreshLayout.setRefreshing(false);
            }
        },1000);
    }
}