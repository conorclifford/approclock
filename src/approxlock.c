#include <string.h>
#include "pebble.h"
#include "writetime.h"

// display digital clock - useful for debugging
#define SHOW_DIGITAL 0

#define BUFSIZE 86

#define SLIDE_IN_OUT_DURATION 600

static Window *window;

struct slideable_watchface_data {
  TextLayer *label;
  GRect *display_rect;
  GRect *off_dest_rect;
  GRect *in_origin_rect;
  char buffer[BUFSIZE];
  char oldbuf[BUFSIZE];
};

static struct slideable_watchface_data main_data;
static struct slideable_watchface_data prec_data;
#if SHOW_DIGITAL
static struct slideable_watchface_data dig_data;
#endif

/*
 * generic animation given a start and finish GRect
 */
void slide_layer(Layer *layer,
                 GRect *start, GRect *finish,
                 int duration,
                 AnimationStoppedHandler stop_handler,
                 struct slideable_watchface_data *context) {
  PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
  animation_set_duration((Animation*) anim, duration);

  AnimationHandlers handlers = {
      .stopped = stop_handler
  };
  animation_set_handlers((Animation*) anim, handlers, context);

  animation_schedule((Animation*) anim);
}

void animate_finished(Animation *anim, bool finished, void *context) {
  property_animation_destroy((PropertyAnimation*) anim);
}

void set_text_and_slide_in(struct slideable_watchface_data *facedata);

void animate_back_in(Animation *anim, bool finished, void *context) {
  struct slideable_watchface_data *facedata = context;
  property_animation_destroy((PropertyAnimation*) anim);
  set_text_and_slide_in(facedata);
}

void slide_out(struct slideable_watchface_data *facedata) {
  slide_layer(
          text_layer_get_layer(facedata->label),
          facedata->display_rect,
          facedata->off_dest_rect,
          SLIDE_IN_OUT_DURATION,
          animate_back_in,
          facedata);
}

void slide_in(struct slideable_watchface_data *facedata) {
  slide_layer(
          text_layer_get_layer(facedata->label),
          facedata->in_origin_rect,
          facedata->display_rect,
          SLIDE_IN_OUT_DURATION,
          animate_finished,
          facedata);
}

void set_text_and_slide_in(struct slideable_watchface_data *facedata) {
  text_layer_set_text(facedata->label, facedata->buffer);
  slide_in(facedata);
}

static int update_if_different(const char *restrict src, char *restrict dest, size_t msize) {
  if (strncmp(src, dest, msize) != 0) {
    strncpy(dest, src, msize);
    return 1;
  }
  return 0;
}

enum time_update_status { none, maintime, precision, both };

static enum time_update_status update_time(struct tm* t) {
  write_time_buffer(t->tm_hour, t->tm_min, main_data.buffer, prec_data.buffer, BUFSIZE);

  enum time_update_status update_status = none;

  if (update_if_different(main_data.buffer, main_data.oldbuf, BUFSIZE)) {
    update_status = maintime;
  }

  if (update_if_different(prec_data.buffer, prec_data.oldbuf, BUFSIZE)) {
    update_status = update_status == none ? precision : both;
  }

  return update_status;
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  enum time_update_status update_status = update_time(tick_time);

  switch (update_status) {
    case maintime:
      slide_out(&main_data);
      break;
    case both:
      slide_out(&main_data);
      slide_out(&prec_data);
      break;
    case precision:
      slide_out(&prec_data);
      break;
    case none: break;
  }
}

#if SHOW_DIGITAL
static void set_dig_time(struct tm *tick_time) {
  snprintf(dig_data.buffer, BUFSIZE, "%02d:%02d.%02d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);
  text_layer_set_text(dig_data.label, dig_data.buffer);
}

static void handle_second_update(struct tm *tick_time, TimeUnits units_changed) {
  set_dig_time(tick_time);
  if (tick_time->tm_sec == 0) handle_minute_tick(tick_time, units_changed);
}
#endif

static void init(void) {
  window = window_create();
  const bool animated = true;
  window_stack_push(window, animated);

  window_set_background_color(window, GColorBlack);

  GFont main_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_INTUITIVE_34));
  GFont prec_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DASHLEY_16));

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  prec_data.display_rect = &GRect(0, 10, frame.size.w, 28);
  prec_data.off_dest_rect = &GRect(0, -48, frame.size.w, 28);
  prec_data.in_origin_rect = prec_data.in_origin_rect;

  prec_data.label = text_layer_create(*(prec_data.off_dest_rect));
  text_layer_set_background_color(prec_data.label, GColorBlack);
  text_layer_set_text_color(prec_data.label, GColorWhite);
  text_layer_set_font(prec_data.label, prec_font);

  main_data.display_rect = &GRect(0, 28, frame.size.w, frame.size.h - 20);
  main_data.off_dest_rect = &GRect(-frame.size.w, 28, frame.size.w, frame.size.h - 20);
  main_data.in_origin_rect = &GRect(frame.size.w, 28, frame.size.w, frame.size.h - 20);
  main_data.label = text_layer_create(*(main_data.off_dest_rect));
  text_layer_set_background_color(main_data.label, GColorBlack);
  text_layer_set_text_color(main_data.label, GColorWhite);
  text_layer_set_font(main_data.label, main_font);

#if SHOW_DIGITAL
  GFont dig_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  dig_data.display_rect = &GRect(0, frame.size.h - 15, frame.size.w, frame.size.h-1);
  dig_data.label = text_layer_create(*(dig_data.display_rect));
  text_layer_set_background_color(dig_data.label, GColorBlack);
  text_layer_set_text_color(dig_data.label, GColorWhite);
  text_layer_set_font(dig_data.label, dig_font);
  text_layer_set_text_alignment(dig_data.label, GTextAlignmentRight);
#endif

  layer_add_child(root_layer, text_layer_get_layer(prec_data.label));
  layer_add_child(root_layer, text_layer_get_layer(main_data.label));
#if SHOW_DIGITAL
  layer_add_child(root_layer, text_layer_get_layer(dig_data.label));
#endif

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);
  set_text_and_slide_in(&prec_data);
  set_text_and_slide_in(&main_data);

#if SHOW_DIGITAL
  set_dig_time(t);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_update);
#else
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
#endif

}

static void destroy(void) {
  window_destroy(window);
  text_layer_destroy(prec_data.label);
  text_layer_destroy(main_data.label);
}

int main(void) {
  init();
  app_event_loop();
  destroy();
}
