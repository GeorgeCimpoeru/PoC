package com.poc.p_couds;

import com.poc.p_couds.pojo.ECU;
import com.poc.p_couds.pojo.UpdateV;
import com.poc.p_couds.pojo.UpdateVResponse;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.POST;

public interface APIInterface {
    @GET("/api/request_ids")
    Call<ECU> requestListOfEcus();

    @POST("/api/update_to_version")
    Call<UpdateVResponse> updateVersion(@Body UpdateV updateV);
}
