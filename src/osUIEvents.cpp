#include "osUIEvents.h"


volatile tosUIEventQueue osUIEventQueue;


ulong osUIEventsInit()
{
  osUIEventQueue.lock     = 1;

  osUIEventQueue.wrIdx    = 0;
  osUIEventQueue.rdIdx    = 0;
  osUIEventQueue.elementCount = 0;

  osUIEventQueue.lock     = 0;

  return 0;
}

//Store event into queue
ulong osPutUIEvent( tosUIEvent *event )
{
  if( !event )
  {
    return 1;
  }

  if( osUIEventQueue.lock )
  {
    return 2;
  }

  osUIEventQueue.lock = 1;

  if( osUIEventQueue.elementCount < ( OS_UI_EVENT_QUEUE_DEPTH - 1 ) )
  {

    osUIEventQueue.queue[ osUIEventQueue.wrIdx ].type = event->type;
    osUIEventQueue.queue[ osUIEventQueue.wrIdx ].arg1 = event->arg1;
    osUIEventQueue.queue[ osUIEventQueue.wrIdx ].arg2 = event->arg2;
    osUIEventQueue.queue[ osUIEventQueue.wrIdx ].obj  = event->obj;

    osUIEventQueue.elementCount++;

    osUIEventQueue.wrIdx++;

    if( osUIEventQueue.wrIdx >= OS_UI_EVENT_QUEUE_DEPTH )
    {
      osUIEventQueue.wrIdx = 0;
    }

    osUIEventQueue.lock = 0;

    return 0;
  }
  else
  {

    osUIEventQueue.lock = 0;

        return 256;

  }
}


//Get event from queue
ulong osGetUIEvent( tosUIEvent *event )
{

  if( !event )
  {
    return 1;
  }

  if( osUIEventQueue.lock )
  {
    return 2;
  }

  osUIEventQueue.lock = 1;

  if( osUIEventQueue.elementCount > 0 )
  {
    event->type = osUIEventQueue.queue[ osUIEventQueue.rdIdx ].type;
    event->arg1 = osUIEventQueue.queue[ osUIEventQueue.rdIdx ].arg1;
    event->arg2 = osUIEventQueue.queue[ osUIEventQueue.rdIdx ].arg2;
    event->obj  = osUIEventQueue.queue[ osUIEventQueue.rdIdx ].obj;


    osUIEventQueue.elementCount--;

    osUIEventQueue.rdIdx++;

    if( osUIEventQueue.rdIdx >= OS_UI_EVENT_QUEUE_DEPTH )
    {
      osUIEventQueue.rdIdx = 0;
    }

    osUIEventQueue.lock = 0;

    return 0;
  }
  else
  {
    event->type = 0;
    event->arg1 = 0;
    event->arg2 = 0;
    event->obj  = 0;

    osUIEventQueue.lock = 0;

    return 256;

  }


  return 0;
}
