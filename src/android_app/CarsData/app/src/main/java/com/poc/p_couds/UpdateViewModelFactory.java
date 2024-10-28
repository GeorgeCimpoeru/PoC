package com.poc.p_couds;

import android.app.Application;

import androidx.annotation.NonNull;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

public class UpdateViewModelFactory implements ViewModelProvider.Factory {
    private final Application application;
    private final String ecuId; // Pass ECU ID or any custom argument

    public UpdateViewModelFactory(Application application, String ecuId) {
        this.application = application;
        this.ecuId = ecuId;
    }

    @NonNull
    @Override
    public <T extends ViewModel> T create(@NonNull Class<T> modelClass) {
        if (modelClass.isAssignableFrom(UpdateViewModel.class)) {
            return (T) new UpdateViewModel(application, ecuId); // Pass the custom argument
        }
        throw new IllegalArgumentException("Unknown ViewModel class");
    }
}
