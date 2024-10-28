package com.poc.p_couds

<<<<<<< HEAD
<<<<<<< HEAD
=======
import com.poc.p_couds.models.IApiService
>>>>>>> development
=======
import com.poc.p_couds.models.IApiService
>>>>>>> 79b55ebf360c9bc84dd23fbef5e73e46935844ae
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

object HandleNetworkReq {
    private const val BASE_URL = "https://vpic.nhtsa.dot.gov/api/vehicles/"

    val api: IApiService by lazy {
        Retrofit.Builder()
            .baseUrl(BASE_URL)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(IApiService::class.java)
    }
}