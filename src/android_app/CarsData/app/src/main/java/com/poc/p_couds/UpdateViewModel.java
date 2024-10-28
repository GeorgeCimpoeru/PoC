package com.poc.p_couds;

import android.app.Application;
import android.os.CountDownTimer;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.poc.p_couds.fragments.Fragment_Update;
import com.poc.p_couds.models.DbHelper;
import com.poc.p_couds.models.IApiService;
import com.poc.p_couds.pojo.UpdateHistory;
import com.poc.p_couds.pojo.UpdateV;
import com.poc.p_couds.pojo.UpdateVResponse;

import java.util.Calendar;
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class UpdateViewModel extends AndroidViewModel {

    // Constants
    private final String TAG = "PoC";

    // Mutable variables
    private final MutableLiveData<Response> updateResponse = new MutableLiveData<>();
    private final MutableLiveData<String> errorMessage = new MutableLiveData<>();
    private final MutableLiveData<Boolean> NewData = new MutableLiveData<>();
    private final MutableLiveData<Integer> progress = new MutableLiveData<>();

    // Variables
    private DbHelper db;
    private IApiService apiInterface;
    private long timeRemaining;     // Track remaining time when paused
    private long totalTimeInMillis;  // Total time for the progress bar
    private CountDownTimer timer;
    private boolean downloading = false;

    public UpdateViewModel(Application application, String ecuId )
    {
        super(application);
        db = new DbHelper(application);
        apiInterface = APIClient.getClient().create(IApiService.class);
    }

    public LiveData<Response> getUpdateResponse() {
        return updateResponse;
    }
    public LiveData<String> getErrorMessage() {
        return errorMessage;
    }
    public LiveData<Boolean> isNewData() {
        return NewData;
    }
    public LiveData<Integer> getProgress()
    {
        return progress;
    }

    // Methods Update history

    public void getListOfUpdatesHistory(String idEcu)
    {
        Call<List<UpdateHistory>> call = apiInterface.requestListOfUpdatesHistory();

        call.enqueue(new Callback<List<UpdateHistory>>() {
            @Override
            public void onResponse(@NonNull Call<List<UpdateHistory>> call, @NonNull Response<List<UpdateHistory>> response) {
                if (response.isSuccessful() && response.body() != null)
                {
                    List<UpdateHistory> listOfUp = response.body();
                    //save in the DB the updates that are not registered
                    for (UpdateHistory update : listOfUp)
                    {
                        db.addNewUpdate(update);
                    }
                    Log.i(TAG, "All good");
                }

                NewData.setValue(true);
            }

            @Override
            public void onFailure(@NonNull Call<List<UpdateHistory>> call, @NonNull Throwable t) {
                Log.e(TAG, "ERROR");
                NewData.setValue(false);
                call.cancel();
            }
        });
    }

    // Methods for updates

    public void updateVersion(UpdateV updateV, String size) {
        Call<UpdateVResponse> call = apiInterface.updateVersion(updateV);
        Log.i(TAG, "Can proceed with download");
        call.enqueue(new Callback<UpdateVResponse>() {
            @Override
            public void onResponse(@NonNull Call<UpdateVResponse> call, @NonNull Response<UpdateVResponse> response) {
                UpdateVResponse updateVResponse = (UpdateVResponse)response.body();

                Calendar calendar = Calendar.getInstance();
                String data = String.valueOf(calendar.get(Calendar.DAY_OF_MONTH)) + "/"
                        + String.valueOf(calendar.get(Calendar.MONTH)) + "/"
                        + String.valueOf(calendar.get(Calendar.YEAR));
                String artifact = "Software update "+ updateV.getUpdateFileVersion() + "(" + db.getAllUpgrades().size() + ")";
                if (response.isSuccessful() &&
                        updateVResponse !=null &&
                        updateVResponse.getErrors() != null &&
                        updateVResponse.getErrors().equals("No errors."))
                {
                    db.addNewUpdate(new UpdateHistory(artifact,"Succeeded",data,size));
                } else {
                    db.addNewUpdate(new UpdateHistory(artifact,"Failed",data,size));
                }
                updateResponse.setValue(response);
            }

            @Override
            public void onFailure(@NonNull Call<UpdateVResponse> call, @NonNull Throwable t) {
                errorMessage.setValue(call.request().url().toString());
                Log.i(TAG, "Finish progress reseting....");
            }
        });
    }

    public void startProgressBar(long durationInMillis)
    {
        downloading = true;

        timeRemaining = durationInMillis;  // Set the time remaining
        totalTimeInMillis = timeRemaining;
        timer = new CountDownTimer(timeRemaining, 100) {  // Update every 100ms
            public void onTick(long millisUntilFinished) {
                if (downloading)
                {
                    timeRemaining = millisUntilFinished;  // Update remaining time
                    int progressV = (int) ((totalTimeInMillis - millisUntilFinished) / (float) totalTimeInMillis * 100);
                    progress.setValue(progressV);
                }
                else {
                    onFinish();
                }
            }

            public void onFinish() {
            }
        }.start();
    }

    public void resumeProgressBar() {
        if (timeRemaining > 0 && progress.getValue() != null && progress.getValue() < 100) {
            startProgressBar(timeRemaining);  // Continue from where we left off
        }
    }

    public void pauseProgressBar()
    {
        timer.cancel();
        progress.setValue(100);
    }

    public void stopProgressBar()
    {
        timer.cancel();
        downloading = false;
    }
}
