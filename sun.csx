#r  "Newtonsoft.Json"
using System.Net;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public static async Task<HttpResponseMessage> Run(HttpRequestMessage req, TraceWriter log)
{
    try
    {
        log.Info("Service 'sun' received a request");
    
        string formattedResponse = await SunRiseAndSetAsync(51.80586,-0.71556);
        
        log.Info("Returning");
        log.Info(formattedResponse);
    
        var response = req.CreateResponse(HttpStatusCode.OK);
        response.Content = new StringContent(formattedResponse);
        
        return response;
    }
    catch (Exception e)
    {
        log.Error(e.ToString());
        throw;
    }

}


public class ApiResponse
        {
            public class ReturnedResults
            {
                [JsonProperty("sunrise")]
                public DateTime Sunrise { get; set; }

                [JsonProperty("sunset")]
                public DateTime Sunset { get; set; }
            }


            [JsonProperty("results")]
            public ReturnedResults Results { get; set; }

            [JsonProperty("status")]
            public string Status { get; set; }

        }

        public static async Task<string> SunRiseAndSetAsync(double lat, double lng)
        {
            HttpClient c = new HttpClient();
            string requestUrl = 
                string.Format("https://api.sunrise-sunset.org/json?lat={0:0.0000000}&lng={1:0.0000000}", lat, lng);

            string jsonResult = await c.GetStringAsync(requestUrl);

            ApiResponse response = JsonConvert.DeserializeObject<ApiResponse>(jsonResult);

            var time = DateTime.UtcNow;
            
            int minutesSince = time.Minute + time.Hour * 60;
            int sunriseMinutesSince = response.Results.Sunrise.Minute + response.Results.Sunrise.Hour * 60;
            int sunsetMinutesSince = response.Results.Sunset.Minute + response.Results.Sunset.Hour * 60;

            return string.Format("CT{0}\r\nSR{1}\r\nSS{2}\r\n", 
                minutesSince,
                sunriseMinutesSince, 
                sunsetMinutesSince);
        }