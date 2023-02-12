#ifndef _dd_dev_h
#define _dd_dev_h

typedef enum dd_return_t {
    DD_RETURN_SUCCESS   = 1,
    DD_RETURN_ERROR     = 2
} dd_return_t;

typedef enum dd_dev_type_t {
    DD_DEV_TYPE_HDD = 0
} dd_dev_type_t;

typedef enum dd_bus_type_t {
    DD_BUS_TYPE_ATA = 0
} dd_bus_type_t;

typedef struct dd_dev_t {
    enum dd_dev_type_t type;
    enum dd_bus_type_t bus_type;

    void *driver_data;
} dd_dev_t;

void dd_dev_add(dd_dev_t dev);
dd_dev_t *dd_dev_get(uint16_t id);
dd_return_t dd_dev_read(dd_dev_t *dev, uint32_t addr, uint32_t size, void *buffer);

#endif