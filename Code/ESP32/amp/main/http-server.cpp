#include "http-server.h"

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"


#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
#include "esp_spiffs.h"

#include "debug.h"
#include <string>
#include "state.h"

#define HTTPD_307 "307 Temporary Redirect"
#define HTTPD_404 "404 Not Found"

HTTP_Server::HTTP_Server( Queue *queue ) : mQueue( queue ), mServerHandle( 0 ) {

}

esp_err_t send_web_response( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_MAIN, req );
}

esp_err_t poweroff( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_POWEROFF, req );
}

esp_err_t poweron( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_POWERON, req );
}

esp_err_t volume_up( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_VOLUME_UP, req );
}

esp_err_t volume_down( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_VOLUME_DOWN, req );
}

esp_err_t volume_up5( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_VOLUME_UP_5, req );
}

esp_err_t volume_down5( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_VOLUME_DOWN_5, req );
}

esp_err_t input_hdmi( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_INPUT_HDMI, req );
}

esp_err_t input_blueray( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_INPUT_BLUERAY, req );
}

esp_err_t input_streamer( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_INPUT_STREAMER, req );
}

esp_err_t input_tv( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_INPUT_TV, req );
}

esp_err_t input_vinyl( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_INPUT_VINYL, req );
}

esp_err_t input_game( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_INPUT_GAME, req );
}

esp_err_t http_404( httpd_req_t *req ) {
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_NOT_FOUND, req );
}



esp_err_t 
HTTP_Server::handleResponse( uint8_t requestType, httpd_req_t *req ) {
    //httpd_resp_set_type(req, "text/html");
    switch( requestType ) {
        case HTTP_Server::SERVER_POWEROFF:
            mQueue->add( Message::MSG_POWEROFF );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;
        case HTTP_Server::SERVER_POWERON:
            mQueue->add( Message::MSG_POWERON );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;
        case HTTP_Server::SERVER_MAIN:
            return httpd_resp_send(req, mMainPage.c_str(), HTTPD_RESP_USE_STRLEN );
            break;
        case HTTP_Server::SERVER_VOLUME_UP:
            mQueue->add( Message::MSG_VOLUME_UP );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;
        case HTTP_Server::SERVER_VOLUME_DOWN:
            mQueue->add( Message::MSG_VOLUME_DOWN );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;
        case HTTP_Server::SERVER_VOLUME_UP_5:
            mQueue->add( Message::MSG_VOLUME_UP, 5 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;
        case HTTP_Server::SERVER_VOLUME_DOWN_5:
            mQueue->add( Message::MSG_VOLUME_DOWN, 5);
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;  
        case HTTP_Server::SERVER_INPUT_HDMI:
            mQueue->add( Message::MSG_INPUT_SET, AmplifierState::INPUT_SPDIF_1 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;  
        case HTTP_Server::SERVER_INPUT_BLUERAY:
            mQueue->add( Message::MSG_INPUT_SET, AmplifierState::INPUT_SPDIF_2 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;      
        case HTTP_Server::SERVER_INPUT_TV:
            mQueue->add( Message::MSG_INPUT_SET, AmplifierState::INPUT_STEREO_1 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;   
        case HTTP_Server::SERVER_INPUT_STREAMER:
            mQueue->add( Message::MSG_INPUT_SET, AmplifierState::INPUT_SPDIF_3 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break; 
        case HTTP_Server::SERVER_INPUT_GAME:
            mQueue->add( Message::MSG_INPUT_SET, AmplifierState::INPUT_STEREO_2 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;      
        case HTTP_Server::SERVER_INPUT_VINYL:
            mQueue->add( Message::MSG_INPUT_SET, AmplifierState::INPUT_STEREO_3 );
            httpd_resp_set_status( req, HTTPD_307 );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, NULL, 0 );
            break;                   
        default:
            return httpd_resp_send(req, mMainPage.c_str(), HTTPD_RESP_USE_STRLEN );
            break;
    }

}

void
HTTP_Server::start() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    mServerHandle = 0;
    config.max_uri_handlers = 30;

    if ( httpd_start( &mServerHandle, &config ) == ESP_OK ) { 
        AMP_DEBUG_I( "Web server started" );

        httpd_uri_t uri_get = {0};
        uri_get.uri = "/";
        uri_get.method = HTTP_GET;
        uri_get.handler = send_web_response;
        uri_get.user_ctx = this;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/poweroff";
        uri_get.handler = poweroff;
        httpd_register_uri_handler( mServerHandle, &uri_get );
        uri_get.uri = "/poweron";
        uri_get.handler = poweron;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/volumeup";
        uri_get.handler = volume_up;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/volumedown";
        uri_get.handler = volume_down;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/volumeup5";
        uri_get.handler = volume_up5;
        httpd_register_uri_handler( mServerHandle, &uri_get );

         uri_get.uri = "/volumedown5";
        uri_get.handler = volume_down5;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/input_hdmi";
        uri_get.handler = input_hdmi;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/input_blueray";
        uri_get.handler = input_blueray;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/input_streamer";
        uri_get.handler = input_streamer;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/input_tv";
        uri_get.handler = input_tv;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/input_vinyl";
        uri_get.handler = input_vinyl;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/input_game";
        uri_get.handler = input_game;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/ipp/print";
        uri_get.method = HTTP_POST;
        uri_get.handler = http_404;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/ipp/print";
        uri_get.method = HTTP_GET;
        uri_get.handler = http_404;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/ipp/print";
        uri_get.method = HTTP_HEAD;
        uri_get.handler = http_404;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/*";
        uri_get.method = HTTP_GET;
        uri_get.handler = http_404;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/*";
        uri_get.method = HTTP_POST;
        uri_get.handler = http_404;
        httpd_register_uri_handler( mServerHandle, &uri_get );
                
        esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
        };

        esp_err_t ret = esp_vfs_spiffs_register(&conf);

        if (ret == ESP_OK) {
            AMP_DEBUG_I( ".. file system mounted" );

            FILE *f = fopen( "/spiffs/index.html", "rt" );
            if ( f ) {
                char s[ 2048 ];

                while ( !feof( f ) ) {
                    fgets( s, 1024, f );

                    if ( strlen( s ) ) {
                        mMainPage = mMainPage + s;
                    }
                }
            }
        }
    }


    
}

void 
HTTP_Server::stop() {

}