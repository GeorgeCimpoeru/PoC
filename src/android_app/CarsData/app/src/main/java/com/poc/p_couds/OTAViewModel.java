package com.poc.p_couds;

import android.app.Application;
import android.util.Log;

import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.poc.p_couds.models.DbHelper;
import com.poc.p_couds.models.IApiService;
import com.poc.p_couds.pojo.FileNode;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class OTAViewModel extends ViewModel {

    private final String TAG = "PoC";

    private final MutableLiveData<String> errorMessageVersions = new MutableLiveData<>();
    private final MutableLiveData<FileNode> listOfVersionResponse = new MutableLiveData<>();

    private final IApiService apiInterface = APIClient.getClient().create(IApiService.class);
    private Call<FileNode> versionCall;

    public LiveData<FileNode> getListOfVersionResponse() {
        return listOfVersionResponse;
    }

    public LiveData<String> getErrorMessageVersions() {
        return errorMessageVersions;
    }

    public void getListOfVersionsAPI()
    {
        if (versionCall != null && versionCall.isExecuted()) {
            versionCall.cancel();
        }

        Call<FileNode> call =  apiInterface.requestListOfVersions();
        call.enqueue(new Callback<FileNode>() {
            @Override
            public void onResponse(Call<FileNode> call, Response<FileNode> response) {
                FileNode body = (FileNode)response.body();
                Log.i(TAG, response.code() + "");

                if (response.isSuccessful() && body != null)
                {
                    listOfVersionResponse.setValue(body);
                    Log.i(TAG,"All good");
                }
                errorMessageVersions.setValue("Code response: " + response.code());
            }

            @Override
            public void onFailure(Call<FileNode> call, Throwable t) {
                if (call.isCanceled()) {
                    Log.i(TAG, "Request was cancelled");
                    return;
                }
                errorMessageVersions.setValue(call.request().url().toString()); // Handle error
            }
        });
    }
}
