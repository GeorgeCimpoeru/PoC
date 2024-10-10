package com.poc.p_couds;

import com.poc.p_couds.pojo.Authenticate;
import com.poc.p_couds.pojo.ChangeStatus;
import com.poc.p_couds.pojo.ECU;
import com.poc.p_couds.pojo.FileNode;
import com.poc.p_couds.pojo.GetIdentifiers;
import com.poc.p_couds.pojo.ReadAccesTiming;
import com.poc.p_couds.pojo.ReadAccessTimingPost;
import com.poc.p_couds.pojo.ReadDtc;
import com.poc.p_couds.pojo.ResetEcu;
import com.poc.p_couds.pojo.ResetEcuPost;
import com.poc.p_couds.pojo.TesterPresent;
import com.poc.p_couds.pojo.UpdateHistory;
import com.poc.p_couds.pojo.UpdateV;
import com.poc.p_couds.pojo.UpdateVResponse;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.POST;

public interface APIInterface {
    @GET("/api/request_ids")
    Call<ECU> requestListOfEcus();

    @POST("/api/update_to_version")
    Call<UpdateVResponse> updateVersion(@Body UpdateV updateV);

    @GET("/api/drive_update_data")
    Call<FileNode> requestListOfVersions();

    @GET("/api/history_updates")
    Call<List<UpdateHistory>> requestListOfUpdatesHistory();

    @GET("/api/change_session")
    Call<ChangeStatus> requestChangeStatus();

    @GET("/api/read_dtc_info")
    Call<ReadDtc> requestReadDtcInfo();

    @GET("/api/authenticate")
    Call<Authenticate> requestAuthenticate();

    @POST("/api/read_access_timing")
    Call<ReadAccesTiming> requestReadAccessTiming(@Body ReadAccessTimingPost readAccessTimingPost);

    @GET("/api/tester_present")
    Call<TesterPresent> requestTesterPresent();

    @POST("/api/reset_ecu")
    Call<ResetEcu> requestResetEcu(@Body ResetEcuPost resetEcuPost);

    @GET("/api/get_identifiers")
    Call<GetIdentifiers> requestGetIdentifiers();
}