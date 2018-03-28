using System.Net;
using System.Text;

public static async Task<HttpResponseMessage> Run(HttpRequestMessage req, TraceWriter log)
{
    log.Info("Mocking the sun");

    var response = req.CreateResponse(HttpStatusCode.OK);
    StringBuilder s = new StringBuilder();
    s.AppendLine("CT10");
    s.AppendLine("SR20");
    s.AppendLine("SS30");
    response.Content = new StringContent(s.ToString());
        
    return response;
    
}
